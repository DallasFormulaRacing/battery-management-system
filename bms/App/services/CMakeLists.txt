# program/CMakeLists.txt

# Create the final firmware executable
add_library(services STATIC 
    bms.c 
    state.c
)

target_include_directories(services PUBLIC .)
target_link_libraries(services PRIVATE thermal cb adbms charger segment)
