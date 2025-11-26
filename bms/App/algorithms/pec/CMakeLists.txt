add_library(bms_pec
    pec.c
    pec.h
)

target_include_directories(bms_pec
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bms_pec
    PUBLIC
        bms_core
)
