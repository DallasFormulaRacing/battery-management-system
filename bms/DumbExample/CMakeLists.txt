add_library(DumbExample_lib STATIC
    DumbExample.c
)

target_include_directories(DumbExample_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
