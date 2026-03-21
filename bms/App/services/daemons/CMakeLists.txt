# services/daemons/CMakeLists.txt

add_library(daemons STATIC 
    logd.c 
)

target_include_directories(daemons PUBLIC .)
target_link_libraries(daemons PRIVATE )
