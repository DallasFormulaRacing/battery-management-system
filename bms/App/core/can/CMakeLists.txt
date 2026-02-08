# Auto-generated CMake for Flat Module: can
add_library(can STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(can PRIVATE ${SRC_FILES})
target_link_libraries(can PUBLIC stm32_hal)
target_include_directories(can PUBLIC .)
