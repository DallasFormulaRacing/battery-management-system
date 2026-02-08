# Auto-generated CMake for Flat Module: eeprom
add_library(eeprom STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(eeprom PRIVATE ${SRC_FILES})

target_include_directories(eeprom PUBLIC .)
