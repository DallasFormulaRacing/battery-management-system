# Auto-generated CMake for Flat Module: fdcan
add_library(fdcan STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(fdcan PRIVATE ${SRC_FILES})
target_link_libraries(fdcan PUBLIC stm32_hal)
target_include_directories(fdcan PUBLIC .)
