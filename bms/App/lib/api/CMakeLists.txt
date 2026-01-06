# App/lib/api/CMakeLists.txt

target_sources(bms_api
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source/api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/source/data_read_handlers.c
        ${CMAKE_CURRENT_SOURCE_DIR}/source/parse.c
        ${CMAKE_CURRENT_SOURCE_DIR}/source/data.c
)

target_include_directories(bms_api
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/App/algorithms/pec/
)

target_link_libraries(bms_api
    PRIVATE
        bms_pec
)
