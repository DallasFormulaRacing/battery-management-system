add_library(bms_cb
    cb.c
    cb.h
    queue.c
    queue.h
)

target_include_directories(bms_cb
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bms_cb
    PUBLIC
        bms_api
        
)
