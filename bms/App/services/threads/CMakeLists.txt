# services/threads/CMakeLists.txt

add_library(threads STATIC 
    safety/safety_monitor.c
    vcan/gui_can_job.c
    tcan/can2_job.c
)

target_include_directories(threads PUBLIC 
    .
    vcan
    tcan
    safety
)
target_link_libraries(threads PRIVATE program fdcan can imd gui freertos elcon thermal cb adbms charger segment)