# App/api/charger/CMakeLists.txt
add_library(charger STATIC)
file(GLOB SRC_FILES "*.c")
target_sources(charger PRIVATE ${SRC_FILES})

target_include_directories(charger PUBLIC .)

target_link_libraries(charger 
    PUBLIC 
        adbms
        cb
        segment
)