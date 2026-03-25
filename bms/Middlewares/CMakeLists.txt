# Middlewares/Third_Party/FreeRTOS/Source/CMakeLists.txt
add_library(freertos STATIC)
set(FREERTOS_SOURCE_DIR Third_Party/FreeRTOS/Source/)


target_sources(freertos PRIVATE
    ${FREERTOS_SOURCE_DIR}/tasks.c
    ${FREERTOS_SOURCE_DIR}/list.c
    ${FREERTOS_SOURCE_DIR}/queue.c
    ${FREERTOS_SOURCE_DIR}/timers.c
    ${FREERTOS_SOURCE_DIR}/event_groups.c
    ${FREERTOS_SOURCE_DIR}/stream_buffer.c

    ${FREERTOS_SOURCE_DIR}/CMSIS_RTOS_V2/cmsis_os2.c
)


target_sources(freertos PRIVATE
    ${FREERTOS_SOURCE_DIR}/portable/GCC/ARM_CM4F/port.c
    ${FREERTOS_SOURCE_DIR}/portable/MemMang/heap_4.c
)

target_compile_definitions(freertos PUBLIC 
    STM32G474xx        
    USE_HAL_DRIVER 
    USE_FreeRTOS
)


target_include_directories(freertos 
    PUBLIC 
        ${FREERTOS_SOURCE_DIR}/include
        ${FREERTOS_SOURCE_DIR}/portable/GCC/ARM_CM4F
        ${FREERTOS_SOURCE_DIR}/CMSIS_RTOS_V2
        "${CMAKE_SOURCE_DIR}/Core/Inc"  
        "${CMAKE_SOURCE_DIR}/Drivers/STM32G4xx_HAL_Driver/Inc"
        "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include"
        "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32G4xx/Include"
        "${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include"

)


target_compile_definitions(freertos PUBLIC USE_FreeRTOS)