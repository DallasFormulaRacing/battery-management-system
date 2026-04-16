# Auto-generated CMake for Library Module: imd
add_library(imd STATIC)

file(GLOB SRC_FILES "src/*.c")
target_sources(imd PRIVATE ${SRC_FILES})

target_include_directories(imd PUBLIC . inc can)
target_link_libraries(imd PUBLIC can)
