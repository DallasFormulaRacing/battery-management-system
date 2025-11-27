# App/lib/program/CMakeLists.txt

target_sources(bms_api
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/bms.c      # or program.c if that’s correct
)

target_include_directories(bms_api
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}            # for bms.h
)
