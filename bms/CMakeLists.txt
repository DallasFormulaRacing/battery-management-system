# ! /CMakeLists.txt -- Root CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_LIST_DIR}/cmake/gcc-arm-none-eabi.cmake"
        CACHE FILEPATH "Toolchain file"
    )
endif()

project(bms C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_SYSROOT /Applications/ArmGNUToolchain/14.3.rel1/arm-none-eabi)

set(CMAKE_C_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")
set(CMAKE_CXX_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")

# Sources

add_subdirectory(App)

# CubeMX-generated application sources
file(GLOB CORE_SOURCES
    Core/Src/*.c
)

add_executable(${PROJECT_NAME}
    ${CORE_SOURCES}
    startup_stm32g474xx.s
)

# MCU / HAL / CMSIS include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    Core/Inc
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32G4xx/Include
    Drivers/STM32G4xx_HAL_Driver/Inc
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        bms_api
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
    --sysroot=/Applications/ArmGNUToolchain/14.3.rel1/arm-none-eabi/arm-none-eabi
)

target_link_options(${PROJECT_NAME} PRIVATE
    -Wl,--gc-sections
)
