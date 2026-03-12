# Auto-generated CMake for Flat Module: emulator
add_library(emulator STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(emulator PRIVATE ${SRC_FILES})

target_include_directories(emulator PUBLIC . lib)
target_link_libraries(emulator PRIVATE adbms fdcan)
