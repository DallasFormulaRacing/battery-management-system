# Middlewares/Third_Party/FreeRTOS/Source/CMakeLists.txt
add_library(freertos STATIC)


target_sources(freertos PRIVATE
    tasks.c
    list.c
    queue.c
    timers.c
    event_groups.c
    stream_buffer.c

    CMSIS_RTOS_V2/cmsis_os2.c
)


target_sources(freertos PRIVATE
    portable/GCC/ARM_CM4F/port.c
    portable/MemMang/heap_4.c
)

target_compile_definitions(freertos PUBLIC 
    STM32G474xx        
    USE_HAL_DRIVER 
    USE_FreeRTOS
)


target_include_directories(freertos 
    PUBLIC 
        include
        portable/GCC/ARM_CM4F
        CMSIS_RTOS_V2
        "${CMAKE_SOURCE_DIR}/Core/Inc"  
        "${CMAKE_SOURCE_DIR}/Drivers/STM32G4xx_HAL_Driver/Inc"
        "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include"
        "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32G4xx/Include"
        "${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include"

)


target_compile_definitions(freertos PUBLIC USE_FreeRTOS)