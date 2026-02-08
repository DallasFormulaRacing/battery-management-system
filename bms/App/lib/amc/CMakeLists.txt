# Auto-generated CMake for Library Module: amc
add_library(amc STATIC)

file(GLOB SRC_FILES "src/*.c")
target_sources(amc PRIVATE ${SRC_FILES})

target_include_directories(amc PUBLIC inc)
