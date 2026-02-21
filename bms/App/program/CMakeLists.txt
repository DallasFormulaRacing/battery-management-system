# program/CMakeLists.txt

# Create the final firmware executable
add_library(bms_app STATIC 
    bms.c 
    state.c
)

target_include_directories(bms_app PUBLIC .)
target_link_libraries(bms_app PRIVATE cb adbms charger segment fdcan)
