# core/CMakeLists.txt

file(GLOB HAL_SOURCES "${CMAKE_SOURCE_DIR}/Drivers/STM32G4xx_HAL_Driver/Src/*.c")
add_library(stm32_hal STATIC ${HAL_SOURCES})

target_compile_definitions(stm32_hal
    PUBLIC
        STM32G474xx
        USE_HAL_DRIVER
)

target_include_directories(stm32_hal
    PUBLIC
        "${CMAKE_SOURCE_DIR}/Core/Inc"
        "${CMAKE_SOURCE_DIR}/Drivers/STM32G4xx_HAL_Driver/Inc"
        "${CMAKE_SOURCE_DIR}/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy"
        "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32G4xx/Include"
        "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include"
)

add_subdirectory(adc)
add_subdirectory(can)
add_subdirectory(fdcan)
add_subdirectory(i2c)
add_subdirectory(spi)
add_subdirectory(tim)

add_library(core_layer INTERFACE)
target_link_libraries(core_layer INTERFACE adc can fdcan i2c spi tim stm32_hal)