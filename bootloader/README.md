# ezfd CAN bootloader for the BMS (STM32G474RE)

A small program that sits in the first 32 KB of flash. On every reset it waits
100 ms for the dashboard to ask for a firmware update, then starts the BMS app.
With it, the BMS can be reflashed over the main CAN bus (dashboard Flash tab)
without opening the battery container.

It only starts an app that is complete and built for this board, reads every
write back, checks a CRC of the whole image before the dashboard starts it,
and runs the watchdog. See [What it checks](#what-it-checks).

You only flash the bootloader once per board, over SWD. After that, every app
build must be made with the bootloader presets (`Debug-BL` / `Release-BL`).

## Flash layout

| Region     | Address    | Size                                  |
|------------|------------|---------------------------------------|
| Bootloader | 0x08000000 | 32 KB                                 |
| BMS app    | 0x08008000 | 224 KB (flash bank 1 ends 0x08040000) |

The bootloader talks on FDCAN2 (the main bus) at 1 Mbit/s, CAN FD, no BRS, as
node 0x03. While it runs it sends the ev27 `BMS_Status` frame (ID 0x703) with
Mode = Bootloader every 500 ms, so the dashboard shows it.

## 1. One-time setup

You need everything from the main [README](../README.md) (CMake, Ninja,
`arm-none-eabi-gcc`) plus:

1. **Rust.** Install rustup from <https://rustup.rs>. On Windows, accept the
   installer's offer to install the Visual Studio C++ build tools; Rust needs
   them for the build script. You don't pick a Rust version: the first `cargo`
   command in this folder reads `rust-toolchain.toml` and installs Rust 1.92 and
   the `thumbv7em-none-eabihf` (Cortex-M4F) target by itself.
2. **probe-rs** (flashes over the Nucleo's ST-LINK or any SWD probe). Follow
   <https://probe.rs/docs/getting-started/installation/>. On Windows that is one
   PowerShell line:
   ```
   irm https://github.com/probe-rs/probe-rs/releases/latest/download/probe-rs-tools-installer.ps1 | iex
   ```
   Check it with `probe-rs list` while the board is plugged in.
3. **Check `bootloader/.cargo/config.toml` exists.** It tells cargo to build for
   the STM32 instead of your PC. If it's missing, `cargo build` tries to build
   for your PC and fails with errors from the embassy crates or the linker. It
   contains:
   ```
   [target.thumbv7em-none-eabihf]
   runner = 'probe-rs run --chip STM32G474RETx'

   [build]
   target = "thumbv7em-none-eabihf"
   ```

The first build downloads Embassy (the Rust HAL) from GitHub, so it needs
internet and takes a few minutes. Later builds take seconds.

## 2. Build and flash the bootloader (once per board)

Run from the `bootloader` folder. Pick the line for your board:

```
cd bootloader
cargo build --release --features nucleo   # bench: Nucleo-G474RE, FDCAN2 on PB12 RX / PB13 TX
cargo build --release                     # BMS controller board, FDCAN2 on PB5 RX / PB6 TX
```

Then erase the chip and flash it:

```
probe-rs erase --chip STM32G474RETx
probe-rs download --chip STM32G474RETx target/thumbv7em-none-eabihf/release/ezfd-bootloader-g474
probe-rs reset --chip STM32G474RETx
```

The erase matters: it removes any old app at 0x08000000, which the bootloader
would otherwise be written over. Release is about 17 KB.

The `--features nucleo` choice is also the bootloader's board tag: a Nucleo
bootloader only starts apps built with `BMS_BOARD_NUCLEO`, a controller one
only `BMS_BOARD_CONTROLLER` (`bms/App/config/bms_board.h`).

**Updating from bootloader 0.1:** flash the new bootloader the same way, then
load an app built from current code. Apps built before the image header
existed are not started (the Status shows "no valid app").

For a debug build that prints logs in the terminal, use
`cargo run --features nucleo` instead of the three probe-rs lines. It flashes,
starts, and shows the log until you press Ctrl+C.

Check: with no app on the chip, `candump can0` on the Pi shows 0x703 every
500 ms with byte 0 = `21` (Mode = Bootloader) and bytes 19-20 = `01 00`
(no valid app). The dashboard shows the BMS in bootloader mode.

## 3. Build the BMS app for the bootloader

Run from the `bms` folder:

```
cd bms
cmake --preset Debug-BL
cmake --build --preset Debug-BL
```

The result is `bms/build/Debug-BL/bms.elf`, linked at 0x08008000. The normal
`Debug` preset still builds the app at 0x08000000 for a board without the
bootloader; flashing that one over SWD overwrites the bootloader.

The -BL builds also get an image header at 0x08008200 (node 0x03, board
`BMS_BOARD`, firmware version) and an end marker as their last 8 bytes, from
`bms/App/config/app_image.c`. Set `BMS_BOARD` in `bms_board.h` to the board
you're flashing; the dashboard shows the board of each uploaded file.

## 4. Load the app

Either way works:

- **Over CAN:** dashboard, Nodes, BMS, **Flash...**, pick `bms.elf`.
- **Over SWD:** this keeps the bootloader, because probe-rs only erases the
  pages it writes:
  ```
  probe-rs download --chip STM32G474RETx build/Debug-BL/bms.elf
  probe-rs reset --chip STM32G474RETx
  ```
  If you flash from VS Code or CubeIDE instead, make sure it is not set to
  erase the whole chip.

After a reset the BMS sends one FwQuery (`04200963`), then about 100 ms later
its normal frames, with Status in Mode = App.

## Rebooting and reflashing over CAN

The dashboard's **Reboot** and **Flash...** buttons work while the app runs. The
app answers the ezfd Ping and Reboot itself. A reboot is refused while a
charging session is active, or when the VCU's `VCU_State.FlashAllowed` is off.
On the bench there is no VCU, so `BMS_BENCH_REBOOT_WITHOUT_VCU` in
`bms/App/services/program/bms.h` skips that check.

## What it checks

1. Clocks: HSI16, no PLL. FDCAN kernel clock = PCLK1 (16 MHz). Starts the
   independent watchdog (30 s).
2. Sends FwQuery and waits 100 ms for the host's answer.
3. If the host doesn't ask it to stay and the app passes the image check, it
   jumps to the app. The image check (`src/image.rs`):
   - stack pointer in RAM, reset vector inside the app slot;
   - image header at 0x08008200: magic 0xFEEDFEED, node 0x03 and this
     bootloader's board;
   - the end marker the header points at (0xBEEFBEEF twice) is written. The
     dashboard writes the file front to back, so an upload that stopped half
     way has no end marker.
4. Otherwise it stays and answers Ping, Erase, Write, Verify, Jump and Reboot.
   It never erases or writes below 0x08008000, so it can't overwrite itself.
   - Every Write is read back before WriteOk goes out.
   - Verify (0x4E) computes a CRC-32 of the image. The dashboard sends it
     after the last write and only sends Jump if it matches.
   - Jump is refused unless the image check passes.
   - With a good app in flash, 30 s without a frame from the host sends it back
     to the app (dashboard closed before the update started).

Before the erase the dashboard also compares the file's header with the board
byte in the bootloader's Ping reply, so a file for the other board, or for
another node, is refused with nothing erased.

### Watchdog

The bootloader starts the IWDG with 30 s, and it keeps running in the app:
once started it can only be stopped by a reset. The app
(`bms/App/services/watchdog/`) watches only its state machine:

- If the state machine hasn't run for 20 s, the app pulls the SDC pin low,
  sends one `BMS_State` frame with State = Fault, FaultCode = 14
  (WatchdogRestart) and ShutdownOpen = 1, then restarts.
- The app sets the IWDG to 25 s and reloads it from its 1 ms tick interrupt,
  so the IWDG only fires when interrupts have stopped too (hard fault,
  `Error_Handler()`). That reset sends no frame; the SDC pin's pull-down opens
  the shutdown circuit.

Missing hardware on the bench makes the state machine fault, not stop, so it
doesn't trip the watchdog. Under a debugger both are frozen whenever the core
is halted.

### Status ErrorFlags (0x703 bytes 19-20, while in the bootloader)

| Bit | Meaning |
|---|---|
| 0 | No app it will start (bits 1-2 say why, if known) |
| 1 | The app in flash is for another node or board |
| 2 | End marker missing: the last upload didn't finish |
| 3 | A write failed or read back wrong since the last Erase |
| 4 | The last reset was the IWDG (seen only if it then stays). The app's 20 s watchdog is a software reset and doesn't set this |

## Troubleshooting

| Problem | Fix |
|---|---|
| `cargo build` fails with embassy or linker errors about your PC's target | `.cargo/config.toml` is missing (step 1.3) |
| `probe-rs` finds no probe | Plug in the Nucleo's USB; on Windows install the ST-LINK driver (comes with STM32CubeProgrammer) |
| App doesn't start after flashing over SWD | It was built with `Debug`, not `Debug-BL`. Rebuild with the `-BL` preset |
| Dashboard Flash refuses the file | The ELF isn't linked at 0x08008000: same fix |
| Bootloader gone after flashing the app | The flasher erased the whole chip. Redo step 2 |
| Dashboard: "this file has none" (no image header) | Built from old code or without a -BL preset. Rebuild with `Debug-BL` |
| Dashboard: "built for the controller board, but ... Nucleo" (or the reverse) | Set `BMS_BOARD` in `bms_board.h` to the board you're flashing and rebuild. Nothing was erased |
| Dashboard: "Verify failed: CRC mismatch" | A frame was corrupted on the way. The app was not started; flash again |
| App flashed over SWD doesn't start, ErrorFlags bit 1 set | The app's `BMS_BOARD` doesn't match the bootloader's board (`--features nucleo` or not) |
| BMS keeps rebooting (Status Uptime keeps dropping to 0) | Look in the log for `BMS_State` with FaultCode 14 just before each restart: the state machine stopped for 20 s. No such frame: the IWDG (interrupts stopped, e.g. a hard fault). Debug with the probe attached; both watchdogs pause at breakpoints |
