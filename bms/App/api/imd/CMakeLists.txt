# Auto-generated CMake for Flat Module: packvolt
add_library(imd STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(imd PRIVATE ${SRC_FILES})

target_include_directories(imd PUBLIC .)
