# services/threads/CMakeLists.txt

add_library(threads STATIC 
    safety_monitor.c
)

target_include_directories(threads PUBLIC .)
target_link_libraries(threads PRIVATE )