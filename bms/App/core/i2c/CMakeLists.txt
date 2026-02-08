# Auto-generated CMake for Flat Module: i2c
add_library(i2c STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(i2c PRIVATE ${SRC_FILES})
target_link_libraries(i2c PUBLIC stm32_hal)
target_include_directories(i2c PUBLIC .)
