# App/lib/api/CMakeLists.txt

target_sources(bms_core
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source/api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/source/comms.c
        ${CMAKE_CURRENT_SOURCE_DIR}/source/parse.c
        # or just parse.c if that’s your real structure
)

target_include_directories(bms_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
