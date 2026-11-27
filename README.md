[![Linux Build](https://github.com/DallasFormulaRacing/battery-management-system/actions/workflows/linux.yml/badge.svg)](https://github.com/DallasFormulaRacing/battery-management-system/actions/workflows/linux.yml)

# Battery Management System

## Documentation

More detailed documentation is $\rightarrow$ [here.](docs/README.md)

We are also working on integrating Doxygen so the BMS can become and remain a multi-year, evolving project.

## Getting Started

Clone with `git clone --recurse-submodules https://github.com/DallasFormulaRacing/battery-management-system.git`

The CAN message definitions come from [ev27-can](https://github.com/DallasFormulaRacing/ev27-can), a submodule at `bms/submodules/ev27-can`. If you already cloned without it, run `git submodule update --init bms/submodules/ev27-can`. Run `git config submodule.recurse true` once so `git pull` and `git checkout` keep it in step.

### Tools

- Git
- CMake 3.22 or newer, and Ninja
- The Arm GNU toolchain (`arm-none-eabi-gcc` on your PATH). On Windows the MSYS2 package `mingw-w64-x86_64-arm-none-eabi-gcc` works.
- [probe-rs](https://probe.rs/docs/getting-started/installation/) to flash over SWD (the Nucleo's ST-LINK works). VS Code with Cortex-Debug or STM32CubeProgrammer also work.
- Only for the CAN bootloader: Rust, see [bootloader/README.md](bootloader/README.md)

DO NOT USE CUBE-CMAKE

### Build

Run from the `bms` folder:

```
cd bms
cmake --preset Debug
cmake --build --preset Debug
```

The firmware is `bms/build/Debug/bms.elf`. The board it is built for (bench Nucleo or controller board) is `BMS_BOARD` in `bms/App/config/bms_board.h`.

| Preset | Use it when | App address |
|---|---|---|
| `Debug` | The chip has no bootloader; you flash over SWD | 0x08000000 |
| `Debug-BL` | The chip has the CAN bootloader; you flash over CAN from the dashboard or over SWD | 0x08008000 |

`Release` and `Release-BL` exist too, but `bms/CMakeLists.txt` currently forces `CMAKE_BUILD_TYPE` to Debug, so they build the same image.

If you get compiler detection errors, check that `arm-none-eabi-gcc` is on your PATH. The presets pass `bms/cmake/gcc-arm-none-eabi.cmake` as the toolchain file.

### Flash

Without the bootloader (`Debug` preset):

```
probe-rs download --chip STM32G474RETx build/Debug/bms.elf
probe-rs reset --chip STM32G474RETx
```

This overwrites the bootloader if the chip has one. To set a board up with the bootloader, and for flashing over CAN, follow [bootloader/README.md](bootloader/README.md).

### Recommended VS Code Extensions

```txt
cschlosser.doxdocgen
dan-c-underwood.arm
marus25.cortex-debug
mcu-debug.debug-tracker-vscode
mcu-debug.memory-view
mcu-debug.peripheral-viewer
mcu-debug.rtos-views
ms-vscode.cmake-tools
ms-vscode.cpptools-extension-pack
ms-vscode.cpptools-themes
ms-vscode.hexeditor
ms-vscode.makefile-tools
ms-vscode.vscode-serial-monitor
pkief.material-icon-theme
trond-snekvik.gnu-mapfiles
twxs.cmake
usernamehw.errorlens
zixuanwang.linkerscript
```

#### Other extensions

To read the documentation in the `docs/` directory, you may need

```
goessner.mdmath
```

### How to Contribute:

New features will be developed on a branch separate from production `master` and will be merged by pull request. All contributions will undergo review at the pull request level.

Your pull request will be automatically blocked from merging if it does not pass a compile check!

Please see [branch naming conventions](https://www.geeksforgeeks.org/git/how-to-naming-conventions-for-git-branches/) and [pull request naming conventions](https://github.com/mozilla-mobile/firefox-ios/wiki/Pull-Request-Naming-Guide) and name them appropriately. Bad naming is bad documentation and bad documentation is bad code. Refer to previous PRs or old branch names for inspiration/clarification.

If applicable, please tie your pull request to one or more issues. Try to find an issue to solve with your PR, or just make one. Duplicate issues will be filtered.

---

> Authors: DALLAS FORMULA RACING EV TEAM
