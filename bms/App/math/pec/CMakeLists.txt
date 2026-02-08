# Auto-generated CMake for Flat Module: pec
add_library(pec STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(pec PRIVATE ${SRC_FILES})

target_include_directories(pec PUBLIC .)
