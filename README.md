[![Linux Build](https://github.com/DallasFormulaRacing/battery-management-system/actions/workflows/linux.yml/badge.svg)](https://github.com/DallasFormulaRacing/battery-management-system/actions/workflows/linux.yml)

# Battery Management System

## Getting Started

Clone with `git clone https://github.com/DallasFormulaRacing/battery-management-system.git`

### Tools

Make sure you have the following utilities:

- `make`
- `gcc-arm-none-eabi`
- `openocd`

[Add more here]

### How to run

1. `make`
2.

### VS Code Extensions

```txt
cschlosser.doxdocgen
dan-c-underwood.arm
marus25.cortex-debug
mcu-debug.debug-tracker-vscode
mcu-debug.memory-view
mcu-debug.peripheral-viewer
mcu-debug.rtos-views
ms-python.python
ms-python.vscode-pylance
ms-python.vscode-python-envs
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

## Directory Structure

### Subdirectories

`App` - contains application layer logic, like algorithms and other user-defined modules. Write additional firmware here.

`Core` - main function and architecture-specific sources. Edit as least often as possible.

`Drivers` - vendor drivers and HAL. Also contains CMSIS library. Do not touch.

### Tree

```bash
.
└── bms
    ├── App
    ├── build
    ├── Core
    │   ├── Inc
    │   └── Src
    └── Drivers
        ├── BSP
        │   └── STM32G4xx_Nucleo
        ├── CMSIS
        │   ├── Device
        │   │   └── ST
        │   │       └── STM32G4xx
        │   │           ├── Include
        │   │           └── Source
        │   │               └── Templates
        │   └── Include
        └── STM32G4xx_HAL_Driver
            ├── Inc
            │   └── Legacy
            └── Src

22 directories

```

---

> Authors: DALLAS FORMULA RACING EV TEAM
