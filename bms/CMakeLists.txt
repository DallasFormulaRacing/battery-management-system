cmake_minimum_required(VERSION 3.22)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

option(STATIC_ANALYSIS "Enable static analysis mode (clang/clang-tidy)" OFF)

if (NOT STATIC_ANALYSIS)
    if (NOT CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE
            "${CMAKE_CURRENT_LIST_DIR}/cmake/gcc-arm-none-eabi.cmake"
            CACHE FILEPATH "ARM GCC toolchain"
        )
    endif()
endif()


project(bms C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(App)

file(GLOB CORE_SOURCES
    Core/Src/*.c
)

file(GLOB HAL_SOURCES
    Drivers/STM32G4xx_HAL_Driver/Src/*.c
)

add_executable(${PROJECT_NAME}
    ${CORE_SOURCES}
    ${HAL_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        Core/Inc
        Drivers/CMSIS/Include
        Drivers/CMSIS/Device/ST/STM32G4xx/Include
    SYSTEM PRIVATE
        Drivers/STM32G4xx_HAL_Driver/Inc
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32G474xx
    USE_HAL_DRIVER
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -ffunction-sections
    -fdata-sections
)

if (NOT STATIC_ANALYSIS)

    target_sources(${PROJECT_NAME} PRIVATE
        startup_stm32g474xx.s
    )

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            bms_api
    )

    target_link_options(${PROJECT_NAME} PRIVATE
        "-T${CMAKE_SOURCE_DIR}/STM32G474XX_FLASH.ld"
        -Wl,--gc-sections
    )

endif()
