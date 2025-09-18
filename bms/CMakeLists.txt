cmake_minimum_required(VERSION 3.22)

project(bms C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Sources
file(GLOB_RECURSE BMS_SOURCES
    Core/Src/*.c
    App/drivers/src/*.c
    App/core/*.c
    App/algorithms/*.c
)

add_executable(${PROJECT_NAME}
    ${BMS_SOURCES}
    startup_stm32g474xx.s
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    Core/Inc
    App/drivers/inc
    App/core
    App/algorithms
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32G4xx/Include
    Drivers/STM32G4xx_HAL_Driver/Inc
    Archive/ADBMS6830/lib/inc
    Archive/ADBMS6830/program/inc
)

# Compiler defines
target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32G474xx
    USE_HAL_DRIVER
)

# Linker script
target_link_options(${PROJECT_NAME} PRIVATE
    "-T${CMAKE_SOURCE_DIR}/STM32G474XX_FLASH.ld"
)

# HAL + CMSIS driver sources
file(GLOB HAL_SOURCES Drivers/STM32G4xx_HAL_Driver/Src/*.c)
target_sources(${PROJECT_NAME} PRIVATE ${HAL_SOURCES})

# Optimization flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -ffunction-sections
    -fdata-sections
)

target_link_options(${PROJECT_NAME} PRIVATE
    -Wl,--gc-sections
)
