# Auto-generated CMake for Flat Module: adc
add_library(adc STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(adc PRIVATE ${SRC_FILES})
target_link_libraries(adc PUBLIC stm32_hal)
target_include_directories(adc PUBLIC .)
