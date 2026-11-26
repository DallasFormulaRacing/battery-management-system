# ezfd bootloader for the EV-27 BMS (STM32G474RE)

Port of the DAQ ezfd CAN FD bootloader to the BMS. It runs on FDCAN2 (the
main bus) at 1 Mbit/s with no BRS, answers as ezfd node 0x03 (the BMS's ev27
node address) and sends the ev27 `BMS_Status` frame (0x703) with
Mode = Bootloader every 500 ms, so the CM5 and the dashboard can see it.

## Flash layout

| Region      | Address      | Size   |
|-------------|--------------|--------|
| Bootloader  | 0x08000000   | 32 KB  |
| BMS app     | 0x08008000   | 224 KB (bank 1 ends at 0x08040000) |

The BMS app is linked for the slot with `cmake -DBOOTLOADER=ON`. That picks
`STM32G474XX_FLASH_BL.ld`, sets `VECT_TAB_OFFSET=0x8000U` and turns on
`BMS_BOOTLOADER=1`, which lets the ev27 Command "reboot to bootloader" reset
the MCU (only when the VCU allows flashing, the BMS is not faulted, not
charging and not balancing). A normal build (`BOOTLOADER=OFF`, the default)
is unchanged and still runs from 0x08000000 without the bootloader.

## Boot sequence

1. Clocks: HSI16, no PLL. FDCAN kernel clock = PCLK1 (16 MHz).
2. Sends FwQuery and waits 100 ms for the host to answer.
3. If the host does not ask it to stay and the app looks valid (stack
   pointer in SRAM, reset vector inside the app slot), it jumps to the app.
4. Otherwise it stays: Status every 500 ms, ErrorFlags bit 0 set when there
   is no valid app.

Commands handled: Ping, Erase (page by page, Status sent between pages),
AddrSize, Write (64-byte chunks, 8-byte aligned, inside the app slot only),
Jump (only with a valid app) and Reboot.

## Build and flash

```
rustup target add thumbv7em-none-eabihf
cargo build --release                    # controller board, FDCAN2 on PB5/PB6
cargo build --release --features nucleo  # bench Nucleo, FDCAN2 on PB12/PB13
probe-rs download --chip STM32G474RETx target/thumbv7em-none-eabihf/release/ezfd-bootloader-g474
```

`cargo run` (debug, RTT logs) also fits the 32 KB slot. Release is about
15 KB, debug about 21 KB.

Flash the bootloader once over SWD, then the app (built with
`-DBOOTLOADER=ON`) over CAN from the dashboard Flash tab, or over SWD at
0x08008000.
