//! ezfd CAN FD bootloader for the EV-27 BMS (STM32G474RE), ported from
//! schommie/ezfd-bootloader-device (STM32H533). Same protocol, so the Pi
//! dashboard's Flash tab and ezfd-bootloader-host work unchanged.
//!
//! Differences from the H533 version:
//!   - Fixed node address 0x03 (ev27-can EV27_ADDR_BMS) instead of a UID table.
//!   - Main bus rules for now: 1 Mbit/s, CAN FD frames, no BRS.
//!   - G4 flash: dual-bank mode (factory default), 2 KB pages, 64-bit writes.
//!     The app slot is the rest of bank 1, 0x08008000-0x0803FFFF (224 KB).
//!   - Checks the app's vector table before jumping and stays in the
//!     bootloader if there is no valid app.
//!   - Sends ev27 Status (0x703, Mode = Bootloader) every 500 ms while it stays.
//!   - A flash error or a bad address skips the WriteOk instead of panicking,
//!     so the host reports the failure and the node is still reachable.
//!   - Answers Reboot (0x4D) with a reset.
//!
//! Boot sequence (as the H533 version): send FirmwareUpdateQuery to the host,
//! wait 100 ms for FirmwareUpdateResponse(1); otherwise jump to the app.
#![no_std]
#![no_main]

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
use embassy_stm32::peripherals::FDCAN2;
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

/// ev27-can EV27_ADDR_BMS. The bootloader address and the Status ID (0x700 + addr).
const THIS_NODE: u16 = 0x03;
const HOST: u16 = CanDevices::RaspberryPi as u16;

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
/// Longest wait for a free TX buffer. Retransmission is off, so a frame nobody
/// ACKs is dropped and the buffer frees up; this only guards against a stuck bus.
const TX_TIMEOUT: Duration = Duration::from_millis(20);

type CanTx = can::CanTx<'static>;
type CanRx = can::CanRx<'static>;

/// The app's vector table looks sane: stack pointer in RAM, reset handler in
/// the app slot (Thumb bit set), and the slot is not erased.
fn app_is_valid() -> bool {
    let sp = unsafe { core::ptr::read_volatile(APP_BASE as *const u32) };
    let reset = unsafe { core::ptr::read_volatile((APP_BASE + 4) as *const u32) };
    let sp_ok = sp > RAM_START && sp <= RAM_END && (sp & 0x3) == 0;
    let reset_ok = (reset & 1) == 1 && (reset & !1) >= APP_BASE + 8 && (reset & !1) < APP_END;
    sp_ok && reset_ok
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

async fn send_status(tx: &mut CanTx, uid: &[u32; 3]) {
    let ecr = pac::FDCAN2.ecr().read();
    let s = status::Status {
        uptime_s: (Instant::now().as_millis() / 1000) as u32,
        error_flags: if app_is_valid() { 0 } else { status::ERR_NO_VALID_APP },
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

    if !stay && app_is_valid() {
        log!("No update requested, jumping to the app");
        unsafe { jump_to_app() };
    }
    log!("Staying in the bootloader (update requested: {}, app valid: {})", stay, app_is_valid());

    let mut flash = flash::Flash::new_blocking(p.FLASH).into_blocking_regions();
    let bank1 = &mut flash.bank1_region;
    let mut chunk_address: u32 = 0;
    let mut chunk_size: u8 = 0;
    let mut next_status = Instant::now();

    loop {
        if Instant::now() >= next_status {
            send_status(&mut tx, &uid).await;
            next_status = Instant::now() + STATUS_PERIOD;
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
        let data = frame.data();

        match command {
            BootloaderCommand::Ping => {
                // 0 = in the bootloader (the app answers 1)
                send(&mut tx, dfr_id(BootloaderCommand::Ping), &[0]).await;
            }
            BootloaderCommand::Erase => {
                let mut ok = true;
                let mut page = APP_BASE;
                while page < APP_END {
                    let off = page - FLASH_BASE;
                    if bank1.blocking_erase(off, off + PAGE_SIZE).is_err() {
                        ok = false;
                        break;
                    }
                    page += PAGE_SIZE;
                    // ~20-40 ms per page, 112 pages: keep the Status going.
                    if Instant::now() >= next_status {
                        send_status(&mut tx, &uid).await;
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
                    continue;
                }
                let mut reply = [0u8; 5];
                reply[..4].copy_from_slice(&chunk_address.to_be_bytes());
                reply[4] = chunk_size;
                send(&mut tx, dfr_id(BootloaderCommand::WriteOk), &reply).await;
                chunk_address += data.len() as u32;
            }
            BootloaderCommand::Jump => {
                if app_is_valid() {
                    log!("Jumping to the app");
                    unsafe { jump_to_app() };
                }
                log!("Jump refused: no valid app at 0x{:08X}", APP_BASE);
            }
            BootloaderCommand::Reboot => cortex_m::peripheral::SCB::sys_reset(),
            _ => log!("Unhandled command 0x{:04X}", m.command),
        }
    }
}
