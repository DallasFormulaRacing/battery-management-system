# services/threads/CMakeLists.txt

add_library(threads STATIC 
    safety_monitor.c
    gui_can_job.c
	can2_job.c
)

target_include_directories(threads PUBLIC .)
target_link_libraries(threads PRIVATE program fdcan can imd gui freertos elcon thermal cb adbms charger segment)