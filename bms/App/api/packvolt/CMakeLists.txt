# Auto-generated CMake for Flat Module: packvolt
add_library(packvolt STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(packvolt PRIVATE ${SRC_FILES})

target_include_directories(packvolt PUBLIC .)
