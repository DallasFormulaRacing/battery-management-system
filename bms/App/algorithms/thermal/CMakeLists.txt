add_library(bms_thermistor
    thermistor.c
    thermistor.h
)

target_include_directories(bms_thermistor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bms_thermistor
    PUBLIC
        bms_api
)
