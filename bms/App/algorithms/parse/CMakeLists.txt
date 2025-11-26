add_library(bms_parse
    parse.c
)

target_include_directories(bms_parse
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bms_parse
    PUBLIC
        bms_core
)
