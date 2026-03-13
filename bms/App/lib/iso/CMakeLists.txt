# Auto-generated CMake for Library Module: iso
add_library(iso STATIC)

file(GLOB SRC_FILES "src/*.c")
target_sources(iso PRIVATE ${SRC_FILES})

target_include_directories(iso PUBLIC inc)
