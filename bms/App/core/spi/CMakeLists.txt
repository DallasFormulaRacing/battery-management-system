# Auto-generated CMake for Flat Module: spi
add_library(spi STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(spi PRIVATE ${SRC_FILES})
target_link_libraries(spi PUBLIC stm32_hal freertos bms_limits)
target_include_directories(spi PUBLIC .)
