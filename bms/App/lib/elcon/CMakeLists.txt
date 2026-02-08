# Auto-generated CMake for Library Module: elcon
add_library(elcon STATIC)

file(GLOB SRC_FILES "src/*.c")
target_sources(elcon PRIVATE ${SRC_FILES})

target_include_directories(elcon PUBLIC inc)
