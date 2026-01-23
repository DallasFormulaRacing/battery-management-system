# App/lib/program/CMakeLists.txt

target_sources(bms_api
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/bms.c   
)

target_include_directories(bms_api
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR} 
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32G4xx_HAL_Driver/Inc       
)

target_link_libraries(bms_api
    PUBLIC
        bms_cb
)
