# Auto-generated CMake for Flat Module: segment
add_library(isolation STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(isolation PRIVATE ${SRC_FILES})

target_include_directories(isolation PUBLIC .)
target_link_libraries(isolation PUBLIC imd gui)