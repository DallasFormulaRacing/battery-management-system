# App/core/cb/CMakeLists.txt
add_library(cb STATIC)

file(GLOB SRC_FILES "*.c")
target_sources(cb PRIVATE ${SRC_FILES})

target_include_directories(cb PUBLIC .)

# FIX: Point to the FOLDER, not the file
target_include_directories(cb PRIVATE "${CMAKE_SOURCE_DIR}/App/lib/adbms/inc")

# Don't forget to hook into the HAL so cb can use stm32g4xx_hal.h
target_link_libraries(cb PUBLIC stm32_hal adbms)