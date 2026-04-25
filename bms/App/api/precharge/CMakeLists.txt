# Auto-generated CMake for Flat Module: packvolt
add_library(precharge STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(precharge PRIVATE ${SRC_FILES})

target_include_directories(precharge PUBLIC .)
target_link_libraries(precharge PUBLIC amc freertos)
