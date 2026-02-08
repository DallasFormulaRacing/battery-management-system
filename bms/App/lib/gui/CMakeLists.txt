# Auto-generated CMake for Library Module: gui
add_library(gui STATIC)

file(GLOB SRC_FILES "src/*.c")
target_sources(gui PRIVATE ${SRC_FILES})

target_include_directories(gui PUBLIC inc)
