# ! App/core/CMakeLists.txt

target_sources(bms_api
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source/bms_driver.c
        ${CMAKE_CURRENT_SOURCE_DIR}/source/comms.c
)

target_compile_definitions(bms_api
    PUBLIC
        STM32G474xx
        USE_HAL_DRIVER
)

target_include_directories(bms_api
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/Core/Inc
        ${CMAKE_SOURCE_DIR}/Drivers/STM32G4xx_HAL_Driver/Inc
        ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32G4xx/Include
        ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)


