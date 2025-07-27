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
