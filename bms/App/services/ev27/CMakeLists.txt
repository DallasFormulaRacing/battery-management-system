# services/ev27/CMakeLists.txt
# Main-bus CAN (FDCAN2) through the ev27-can submodule: port glue, the two CAN
# tasks and the BMS's own messages. Copied from ev27-can's template/ and adapted.

add_library(ev27_bms STATIC
    ev27_port_stm32.c
    ev27_can_tasks.c
    ev27_device.c
    ev27_ezfd.c
)

target_include_directories(ev27_bms PUBLIC .)

target_link_libraries(ev27_bms
    PUBLIC ev27can stm32_hal freertos fdcan
    PRIVATE program adbms cb segment charger elcon thermal bms_limits imd
)
