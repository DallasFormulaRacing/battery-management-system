# App/lib/api/CMakeLists.txt

target_sources(bms_core
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source/api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/source/comms.c
        ${CMAKE_CURRENT_SOURCE_DIR}/source/parse.c
)

target_include_directories(bms_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/App/algorithms/pec/
)

target_link_libraries(bms_core
    PRIVATE
        bms_pec     
)