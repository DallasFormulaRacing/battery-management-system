# Auto-generated CMake for Flat Module: segment
add_library(segment STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(segment PRIVATE ${SRC_FILES})

target_include_directories(segment PUBLIC .)
target_link_libraries(segment PUBLIC adbms)