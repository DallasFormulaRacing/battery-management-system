# App/core/cb/CMakeLists.txt
add_library(cb STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(cb PRIVATE ${SRC_FILES})

target_include_directories(cb PUBLIC .)

target_include_directories(cb PRIVATE "${CMAKE_SOURCE_DIR}/App/lib/adbms/inc")

target_link_libraries(cb PUBLIC stm32_hal adbms)