# services/watchdog/CMakeLists.txt
# 20 s BMS watchdog: SDC open, a BMS_State frame on FDCAN2, then a restart.
# The IWDG is the backstop.

add_library(bms_watchdog STATIC
    bms_watchdog.c
)

target_include_directories(bms_watchdog PUBLIC .)

target_link_libraries(bms_watchdog
    PUBLIC stm32_hal
    PRIVATE ev27can fdcan
)
