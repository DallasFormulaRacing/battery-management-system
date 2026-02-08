# Auto-generated CMake for Library Module: adbms
add_library(adbms STATIC)

file(GLOB SRC_FILES "src/*.c")
target_sources(adbms PRIVATE ${SRC_FILES})

target_include_directories(adbms PUBLIC inc)
target_link_libraries(adbms PUBLIC pec)
target_link_libraries(adbms PUBLIC spi)
