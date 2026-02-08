# Auto-generated CMake for Library Module: nvm
add_library(nvm STATIC)

file(GLOB SRC_FILES "src/*.c")
target_sources(nvm PRIVATE ${SRC_FILES})

target_include_directories(nvm PUBLIC inc)
