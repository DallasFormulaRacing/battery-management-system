# Auto-generated CMake for Flat Module: interface_gui
add_library(interface_gui STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(interface_gui PRIVATE ${SRC_FILES})

target_include_directories(interface_gui PUBLIC . lib)
target_link_libraries(interface_gui PRIVATE adbms fdcan)
