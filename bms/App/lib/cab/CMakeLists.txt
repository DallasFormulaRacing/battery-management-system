# Auto-generated CMake for Library Module: cab
add_library(cab STATIC)

file(GLOB SRC_FILES "src/*.c")
target_sources(cab PRIVATE ${SRC_FILES})

target_include_directories(cab PUBLIC . inc adbms segment cb thermal fdcan)
target_link_libraries(cab PUBLIC program cb pec charger fdcan adbms)
