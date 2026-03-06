# Auto-generated CMake for Flat Module: interface
add_library(interface STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(interface PRIVATE ${SRC_FILES})

target_include_directories(interface PUBLIC . lib)
target_link_libraries(interface PRIVATE adbms fdcan)
