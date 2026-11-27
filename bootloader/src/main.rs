//! ezfd CAN FD bootloader for the EV-27 BMS (STM32G474RE), ported from
//! schommie/ezfd-bootloader-device (STM32H533). Same protocol, so the Pi
//! dashboard's Flash tab and ezfd-bootloader-host work unchanged.
//!
//! Differences from the H533 version:
//!   - Fixed node address 0x03 (ev27-can EV27_ADDR_BMS) instead of a UID table.
//!   - Main bus rules for now: 1 Mbit/s, CAN FD frames, no BRS.
//!   - G4 flash: dual-bank mode (factory default), 2 KB pages, 64-bit writes.
//!     The app slot is the rest of bank 1, 0x08008000-0x0803FFFF (224 KB).
//!   - Starts the app only if it is complete and built for this board: the
//!     vector table, the image header (node and board tag) and the end
//!     marker are checked first (image.rs). Otherwise it stays.
//!   - Reads every write back before it answers WriteOk, and checks a CRC-32
//!     of the whole image on request (Verify, 0x4E).
//!   - Sends ev27 Status (0x703, Mode = Bootloader) every 500 ms while it stays.
//!   - A flash error or a bad address skips the WriteOk instead of panicking,
//!     so the host reports the failure and the node is still reachable.
//!   - Answers Reboot (0x4D) with a reset.
//!   - Goes back to a complete app after 30 s without a frame from the host.
//!   - Runs the independent watchdog (30 s). It keeps running in the app,
//!     which sets it to 25 s in bms_watchdog_start() and reloads it from its
//!     1 ms tick.
//!
//! Ping is answered with [0, board]; the app answers [1, board].
//!
//! Boot sequence (as the H533 version): send FirmwareUpdateQuery to the host,
//! wait 100 ms for FirmwareUpdateResponse(1); otherwise jump to the app.
#![no_std]
#![no_main]

mod crc;
mod image;
mod protocol;
mod status;

use core::num::{NonZeroU8, NonZeroU16};

use embassy_executor::Spawner;
use embassy_stm32::can::config::{
    ClockDivider, DataBitTiming, FdCanConfig, FrameTransmissionConfig, GlobalFilter,
    NominalBitTiming, TxBufferMode,
};
use embassy_stm32::can::filter::{ExtendedFilter, ExtendedFilterSlot, FilterType};
use embassy_stm32::can::frame::FdFrame;
use embassy_stm32::peripherals::{FDCAN2, IWDG};
use embassy_stm32::wdg::IndependentWatchdog;
use embassy_stm32::{Config, bind_interrupts, can, flash, pac, rcc};
use embassy_time::{Duration, Instant, Timer, with_timeout};
use embedded_can::Id;
use panic_halt as _;
use protocol::*;

macro_rules! log {
    ($($arg:tt)*) => {{
        #[cfg(debug_assertions)]
        rtt_target::rprintln!($($arg)*);
    }};
}

bind_interrupts!(struct Irqs {
    FDCAN2_IT0 => can::IT0InterruptHandler<FDCAN2>;
    FDCAN2_IT1 => can::IT1InterruptHandler<FDCAN2>;
});

/// The bootloader address and the Status ID (0x700 + addr).
const THIS_NODE: u16 = EV27_ADDR_BMS;
/// Who flashes us: the CM5 (Pi dashboard, ezfd-bootloader-host).
const HOST: u16 = EV27_ADDR_CM5;

const FLASH_BASE: u32 = 0x0800_0000;
const APP_BASE: u32 = 0x0800_8000;
/// End of bank 1 in dual-bank mode. The dashboard caps apps at 224 KB to match.
const APP_END: u32 = 0x0804_0000;
const PAGE_SIZE: u32 = 2048;
const WRITE_SIZE: usize = 8;
const RAM_START: u32 = 0x2000_0000;
const RAM_END: u32 = 0x2002_0000; // SRAM1 + SRAM2 + CCM SRAM alias, 128 KB
const UID_BASE: u32 = 0x1FFF_7590;

const HOST_WAIT: Duration = Duration::from_millis(100);
const STATUS_PERIOD: Duration = Duration::from_millis(500);
/// With a complete app in flash, a bootloader the host told to stay goes back
/// to it after this long without a frame from the host (the dashboard was
/// closed or the Pi lost power before the update started).
const IDLE_TIMEOUT: Duration = Duration::from_secs(30);
/// Independent watchdog. Petted at least every 50 ms here; after the jump the
/// app sets it to 25 s and reloads it every 1 ms. 30 s is close to the IWDG's
/// longest (32.7 s at /256).
const WDG_TIMEOUT_US: u32 = 30_000_000;
/// Flash read per CRC step, so the watchdog is petted in between.
const CRC_STEP: u32 = 4096;
/// Longest wait for a free TX buffer. Retransmission is off, so a frame nobody
/// ACKs is dropped and the buffer frees up; this only guards against a stuck bus.
const TX_TIMEOUT: Duration = Duration::from_millis(20);

type CanTx = can::CanTx<'static>;
type CanRx = can::CanRx<'static>;
type Watchdog = IndependentWatchdog<'static, IWDG>;

fn app_ok() -> bool {
    image::check() == image::Check::Ok
}

/// ErrorFlags for Status: why the app can't start, plus the latched events.
fn error_flags(write_failed: bool, watchdog_reset: bool) -> u16 {
    let mut f = match image::check() {
        image::Check::Ok => 0,
        image::Check::NoApp => status::ERR_NO_VALID_APP,
        image::Check::WrongBoard => status::ERR_NO_VALID_APP | status::ERR_WRONG_BOARD,
        image::Check::Incomplete => status::ERR_NO_VALID_APP | status::ERR_INCOMPLETE,
    };
    if write_failed {
        f |= status::ERR_WRITE_FAILED;
    }
    if watchdog_reset {
        f |= status::ERR_WATCHDOG_RESET;
    }
    f
}

/// CRC-32 (zlib) of `len` bytes of flash from APP_BASE.
fn flash_crc(len: u32, wdg: &mut Watchdog) -> u32 {
    let mut crc = 0;
    let mut addr = APP_BASE;
    let end = APP_BASE + len;
    while addr < end {
        let n = core::cmp::min(CRC_STEP, end - addr);
        let bytes = unsafe { core::slice::from_raw_parts(addr as *const u8, n as usize) };
        crc = crc::update(crc, bytes);
        addr += n;
        wdg.pet();
    }
    crc
}

fn read_uid() -> [u32; 3] {
    let p = UID_BASE as *const u32;
    unsafe {
        [
            core::ptr::read_volatile(p),
            core::ptr::read_volatile(p.add(1)),
            core::ptr::read_volatile(p.add(2)),
        ]
    }
}

fn dfr_id(command: BootloaderCommand) -> u32 {
    DfrCanId::new(1, HOST, command.into(), THIS_NODE).unwrap().to_raw_id()
}

async fn send(tx: &mut CanTx, id: u32, data: &[u8]) {
    if let Ok(frame) = FdFrame::new_extended(id, data) {
        let _ = with_timeout(TX_TIMEOUT, tx.write_fd(&frame)).await;
    }
}

async fn send_status(tx: &mut CanTx, uid: &[u32; 3], error_flags: u16) {
    let ecr = pac::FDCAN2.ecr().read();
    let s = status::Status {
        uptime_s: (Instant::now().as_millis() / 1000) as u32,
        error_flags,
        tec: ecr.tec(),
        rec: ecr.rec(),
        uid: *uid,
    };
    let buf = status::pack(&s);
    if let Ok(frame) = FdFrame::new_standard(status::STATUS_BASE_ID + THIS_NODE, &buf) {
        let _ = with_timeout(TX_TIMEOUT, tx.write_fd(&frame)).await;
    }
}

/// Waits up to HOST_WAIT for FirmwareUpdateResponse. True = the host wants an update.
///
/// embassy's read_fd() returns a bus error at once, without waiting, whenever
/// the FDCAN has seen one since the last read. Each error is followed by a
/// short sleep so a faulty bus can't keep this loop from ever reaching the
/// deadline: whatever the bus does, the app starts after about 100 ms.
async fn wait_for_host(rx: &mut CanRx) -> bool {
    let deadline = Instant::now() + HOST_WAIT;
    loop {
        let now = Instant::now();
        if now >= deadline {
            return false;
        }
        match with_timeout(deadline - now, rx.read_fd()).await {
            Ok(Ok(message)) => {
                let (frame, _ts) = message.parts();
                if let Id::Extended(id) = frame.id() {
                    let m = parse_can_id(id.as_raw());
                    if m.target == THIS_NODE
                        && BootloaderCommand::try_from(m.command) == Ok(BootloaderCommand::FirmwareUpdateResponse)
                    {
                        return frame.data().first().copied().unwrap_or(0) == 1;
                    }
                }
            }
            Ok(Err(_)) => Timer::after_micros(500).await,
            Err(_) => return false,
        }
    }
}

/// Puts the chip back close to its reset state and starts the app at APP_BASE.
unsafe fn jump_to_app() -> ! {
    unsafe {
        let mut p = cortex_m::Peripherals::steal();
        cortex_m::interrupt::disable();

        // Flush the flash instruction and data caches, so nothing read while
        // the app was being written survives into it. They must be off to be reset.
        let acr = pac::FLASH.acr();
        acr.modify(|w| {
            w.set_icen(false);
            w.set_dcen(false);
        });
        acr.modify(|w| {
            w.set_icrst(true);
            w.set_dcrst(true);
        });
        acr.modify(|w| {
            w.set_icrst(false);
            w.set_dcrst(false);
        });
        acr.modify(|w| {
            w.set_icen(true);
            w.set_dcen(true);
        });

        p.SYST.disable_counter();
        p.SYST.disable_interrupt();
        for i in 0..16 {
            p.NVIC.icer[i].write(0xFFFF_FFFF);
            p.NVIC.icpr[i].write(0xFFFF_FFFF);
        }

        // Reset every peripheral the bootloader may have touched (FDCAN2, the
        // embassy time-driver timer, GPIO), as HAL_DeInit() does, so the app's
        // own init starts from reset values. The clock is still HSI16, which
        // is what the app's SystemClock_Config() expects to find.
        let rcc = pac::RCC;
        rcc.apb1rstr1().write_value(pac::rcc::regs::Apb1rstr1(0xFFFF_FFFF));
        rcc.apb1rstr1().write_value(pac::rcc::regs::Apb1rstr1(0));
        rcc.apb1rstr2().write_value(pac::rcc::regs::Apb1rstr2(0xFFFF_FFFF));
        rcc.apb1rstr2().write_value(pac::rcc::regs::Apb1rstr2(0));
        rcc.apb2rstr().write_value(pac::rcc::regs::Apb2rstr(0xFFFF_FFFF));
        rcc.apb2rstr().write_value(pac::rcc::regs::Apb2rstr(0));
        rcc.ahb1rstr().write_value(pac::rcc::regs::Ahb1rstr(0xFFFF_FFFF));
        rcc.ahb1rstr().write_value(pac::rcc::regs::Ahb1rstr(0));
        rcc.ahb2rstr().write_value(pac::rcc::regs::Ahb2rstr(0xFFFF_FFFF));
        rcc.ahb2rstr().write_value(pac::rcc::regs::Ahb2rstr(0));

        p.SCB.vtor.write(APP_BASE);
        cortex_m::asm::dsb();
        cortex_m::asm::isb();
        // No interrupt is enabled any more. Leaving PRIMASK set would carry
        // into the app and stall HAL_Delay() until FreeRTOS starts.
        cortex_m::interrupt::enable();
        cortex_m::asm::bootload(APP_BASE as *const u32);
    }
}

#[embassy_executor::main]
async fn main(_spawner: Spawner) {
    #[cfg(debug_assertions)]
    rtt_target::rtt_init_print!();

    // HSI16 straight to SYSCLK, no PLL: the app's SystemClock_Config() then
    // starts from the reset clock tree. FDCAN runs from PCLK1 = 16 MHz (its
    // reset source is HSE, which the BMS doesn't have).
    let mut config = Config::default();
    config.rcc.hsi = true;
    config.rcc.sys = rcc::Sysclk::HSI;
    config.rcc.pll = None;
    config.rcc.mux.fdcansel = rcc::mux::Fdcansel::PCLK1;
    let p = embassy_stm32::init(config);

    // Watchdog first. Read the reset flags, then clear them so the next boot
    // only sees its own. The IWDG is frozen while a debugger halts the core.
    let watchdog_reset = pac::RCC.csr().read().iwdgrstf();
    pac::RCC.csr().modify(|w| w.set_rmvf(true));
    pac::DBGMCU.apb1lfzr().modify(|w| w.set_iwdg(true));
    let mut wdg: Watchdog = IndependentWatchdog::new(p.IWDG, WDG_TIMEOUT_US);
    wdg.unleash();

    let uid = read_uid();

    #[cfg(not(feature = "nucleo"))]
    let mut can = can::CanConfigurator::new(p.FDCAN2, p.PB5, p.PB6, Irqs);
    #[cfg(feature = "nucleo")]
    let mut can = can::CanConfigurator::new(p.FDCAN2, p.PB12, p.PB13, Irqs);

    // Only 29-bit frames addressed to this node.
    can.properties().set_extended_filter(
        ExtendedFilterSlot::_0,
        ExtendedFilter {
            filter: FilterType::BitMask { filter: (THIS_NODE as u32) << 21, mask: 0x1F << 21 },
            action: can::filter::Action::StoreInFifo0,
        },
    );

    // 1 Mbit/s from 16 MHz: 16 tq, sample point 75 %, SJW 4. Data phase the
    // same (no BRS on the main bus for now).
    let bit = NominalBitTiming {
        prescaler: NonZeroU16::new(1).unwrap(),
        seg1: NonZeroU8::new(11).unwrap(),
        seg2: NonZeroU8::new(4).unwrap(),
        sync_jump_width: NonZeroU8::new(4).unwrap(),
    };
    let data_bit = DataBitTiming {
        transceiver_delay_compensation: false,
        prescaler: NonZeroU16::new(1).unwrap(),
        seg1: NonZeroU8::new(11).unwrap(),
        seg2: NonZeroU8::new(4).unwrap(),
        sync_jump_width: NonZeroU8::new(4).unwrap(),
    };
    let can_config = FdCanConfig::default()
        .set_clock_divider(ClockDivider::_1)
        .set_frame_transmit(FrameTransmissionConfig::AllowFdCan)
        // Off on purpose: with nobody on the bus a frame that is never ACKed
        // would block the TX FIFO, and the bootloader must still reach the app.
        .set_automatic_retransmit(false)
        .set_transmit_pause(false)
        .set_protocol_exception_handling(false)
        .set_tx_buffer_mode(TxBufferMode::Fifo)
        .set_global_filter(GlobalFilter::reject_all())
        .set_automatic_bus_off_recovery(true)
        .set_nominal_bit_timing(bit)
        .set_data_bit_timing(data_bit);
    can.set_config(can_config);

    let can = can.into_normal_mode();
    let (mut tx, mut rx, _props) = can.split();

    // Ask the host whether to stay.
    send(&mut tx, dfr_id(BootloaderCommand::FirmwareUpdateQuery), &[]).await;
    let stay = wait_for_host(&mut rx).await;

    if !stay && app_ok() {
        log!("No update requested, jumping to the app");
        unsafe { jump_to_app() };
    }
    log!("Staying in the bootloader (update requested: {}, app check: {:?})", stay, image::check());

    let mut flash = flash::Flash::new_blocking(p.FLASH).into_blocking_regions();
    let bank1 = &mut flash.bank1_region;
    let mut chunk_address: u32 = 0;
    let mut chunk_size: u8 = 0;
    let mut next_status = Instant::now();
    let mut last_host = Instant::now();
    let mut write_failed = false;

    loop {
        wdg.pet();
        if Instant::now() >= next_status {
            send_status(&mut tx, &uid, error_flags(write_failed, watchdog_reset)).await;
            next_status = Instant::now() + STATUS_PERIOD;
        }
        if Instant::now() - last_host >= IDLE_TIMEOUT && app_ok() {
            log!("No host for {} s, going back to the app", IDLE_TIMEOUT.as_secs());
            unsafe { jump_to_app() };
        }

        let message = match with_timeout(Duration::from_millis(50), rx.read_fd()).await {
            Ok(Ok(m)) => m,
            Ok(Err(_e)) => {
                Timer::after_micros(500).await; // see wait_for_host()
                continue;
            }
            Err(_) => continue, // timeout: go round to send Status
        };
        let (frame, _ts) = message.parts();
        let Id::Extended(id) = frame.id() else { continue };
        let m = parse_can_id(id.as_raw());
        if m.target != THIS_NODE {
            continue;
        }
        let Ok(command) = BootloaderCommand::try_from(m.command) else {
            log!("Unknown command 0x{:04X}", m.command);
            continue;
        };
        last_host = Instant::now();
        let data = frame.data();

        match command {
            BootloaderCommand::Ping => {
                // [0 = in the bootloader (the app answers 1), board tag]
                send(&mut tx, dfr_id(BootloaderCommand::Ping), &[0, image::BOARD]).await;
            }
            BootloaderCommand::Erase => {
                let mut ok = true;
                let mut page = APP_BASE;
                write_failed = false;
                while page < APP_END {
                    wdg.pet();
                    let off = page - FLASH_BASE;
                    if bank1.blocking_erase(off, off + PAGE_SIZE).is_err() {
                        ok = false;
                        break;
                    }
                    page += PAGE_SIZE;
                    // ~20-40 ms per page, 112 pages: keep the Status going.
                    if Instant::now() >= next_status {
                        send_status(&mut tx, &uid, error_flags(write_failed, watchdog_reset)).await;
                        next_status = Instant::now() + STATUS_PERIOD;
                    }
                }
                if ok {
                    log!("Erase complete");
                    send(&mut tx, dfr_id(BootloaderCommand::EraseOk), &[]).await;
                } else {
                    log!("Erase failed at 0x{:08X}", page);
                }
            }
            BootloaderCommand::AddressAndSize => {
                if data.len() >= 5 {
                    chunk_address = u32::from_be_bytes([data[0], data[1], data[2], data[3]]);
                    chunk_size = data[4];
                }
            }
            BootloaderCommand::Write => {
                // The host pads the frame to its CAN FD length with 0xFF; pad
                // the rest of the last double word with 0xFF too.
                let len = (data.len() + WRITE_SIZE - 1) & !(WRITE_SIZE - 1);
                let in_slot = chunk_address >= APP_BASE
                    && (chunk_address as usize) % WRITE_SIZE == 0
                    && len <= 64
                    && chunk_address + len as u32 <= APP_END;
                if !in_slot {
                    log!("Write refused at 0x{:08X} ({} bytes)", chunk_address, data.len());
                    continue;
                }
                let mut buf = [0xFFu8; 64];
                buf[..data.len()].copy_from_slice(data);
                if bank1.blocking_write(chunk_address - FLASH_BASE, &buf[..len]).is_err() {
                    // Usually a second write to the same place without an erase.
                    log!("Flash write failed at 0x{:08X}", chunk_address);
                    write_failed = true;
                    continue;
                }
                // Read it back (the driver resets the data cache after a write).
                let written = unsafe { core::slice::from_raw_parts(chunk_address as *const u8, len) };
                if written != &buf[..len] {
                    log!("Read-back mismatch at 0x{:08X}", chunk_address);
                    write_failed = true;
                    continue;
                }
                let mut reply = [0u8; 5];
                reply[..4].copy_from_slice(&chunk_address.to_be_bytes());
                reply[4] = chunk_size;
                send(&mut tx, dfr_id(BootloaderCommand::WriteOk), &reply).await;
                chunk_address += data.len() as u32;
            }
            BootloaderCommand::Verify => {
                // [length u32 BE, crc32 u32 BE] -> [result, flash crc32 BE, image check]
                let mut result = 2u8; // bad length
                let mut crc = 0u32;
                if data.len() >= 8 {
                    let len = u32::from_be_bytes([data[0], data[1], data[2], data[3]]);
                    let want = u32::from_be_bytes([data[4], data[5], data[6], data[7]]);
                    if len > 0 && len <= APP_END - APP_BASE {
                        crc = flash_crc(len, &mut wdg);
                        result = if crc != want {
                            1
                        } else if !app_ok() {
                            3
                        } else {
                            0
                        };
                    }
                }
                log!("Verify: result {}, flash crc 0x{:08X}", result, crc);
                let mut reply = [0u8; 6];
                reply[0] = result;
                reply[1..5].copy_from_slice(&crc.to_be_bytes());
                reply[5] = image::check() as u8;
                send(&mut tx, dfr_id(BootloaderCommand::VerifyResult), &reply).await;
            }
            BootloaderCommand::Jump => {
                if app_ok() {
                    log!("Jumping to the app");
                    unsafe { jump_to_app() };
                }
                log!("Jump refused: app check {:?}", image::check());
            }
            BootloaderCommand::Reboot => cortex_m::peripheral::SCB::sys_reset(),
            _ => log!("Unhandled command 0x{:04X}", m.command),
        }
    }
}
