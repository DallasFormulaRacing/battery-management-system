# Auto-generated CMake for Flat Module: thermal
add_library(thermal STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(thermal PRIVATE ${SRC_FILES})

target_include_directories(thermal PUBLIC .)
