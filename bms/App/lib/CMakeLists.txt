add_library(bms_lib
    bms.c
    parse.c
)

target_compile_definitions(bms_lib
    PUBLIC
        STM32G474xx    
        USE_HAL_DRIVER
)

target_include_directories(bms_lib
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/Core/Inc   
        ${CMAKE_SOURCE_DIR}/App/core/include
        ${CMAKE_SOURCE_DIR}/Core/Inc
        ${CMAKE_SOURCE_DIR}/Drivers/STM32G4xx_HAL_Driver/Inc
        ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32G4xx/Include
        ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include   
)

