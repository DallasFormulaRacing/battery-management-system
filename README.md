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

## Directory Structure

### Subdirectories

`App` - contains application layer logic, like algorithms, and user-defined modules.

`Core` - main function and project specific sources.

`Drivers` - vendor drivers and HAL. Also contains CMSIS

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

> DALLAS FORMULA RACING EV TEAM
