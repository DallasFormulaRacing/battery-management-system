# program/CMakeLists.txt

add_library(program STATIC 
    bms.c 
    state.c
    supervisor.c
)

target_include_directories(program PUBLIC .)
target_link_libraries(program PRIVATE thermal cb adbms charger segment elcon)


