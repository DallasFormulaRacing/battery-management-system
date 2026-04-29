# Auto-generated CMake for Flat Module: eeprom
add_library(eeprom STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(eeprom PRIVATE ${SRC_FILES})
target_link_libraries(eeprom PUBLIC stm32_hal)
target_include_directories(eeprom PUBLIC .)