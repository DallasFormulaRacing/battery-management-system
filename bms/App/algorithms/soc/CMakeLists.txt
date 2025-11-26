add_library(bms_soc
     ${CMAKE_CURRENT_SOURCE_DIR}/coulomb.c
)

target_include_directories(bms_soc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bms_soc
    PUBLIC
        bms_core
)
