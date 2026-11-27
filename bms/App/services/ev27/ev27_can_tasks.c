/*
 * The two main-bus CAN tasks (from the ev27-can template). Generic: TimeSync,
 * Commands, Mute and Status go through ev27_node, everything else through the
 * ev27_device_* hooks in ev27_device.c. Put BMS messages there, not here.
 *
 * Both are created in main.c (USER CODE RTOS_THREADS) after ev27_can_start().
 */
#include "cmsis_os2.h"
#include "main.h"

#include "ev27_device_config.h"
#include "ev27_main.h"
#include "ev27_node.h"
#include "ev27_port.h"

const osThreadAttr_t ev27_can_rx_task_attributes = {
    .name = "ev27_can_rx",
    .priority = (osPriority_t)osPriorityAboveNormal,
    .stack_size = 1024};

/* Above the BMS state machine (Normal) so the 10 ms cadence holds while it
 * does long SPI work. Each tick is short: at most a couple of frames. */
const osThreadAttr_t ev27_can_tx_task_attributes = {
    .name = "ev27_can_tx",
    .priority = (osPriority_t)osPriorityAboveNormal,
    .stack_size = 2048};

void ev27_can_rx_task(void *argument)
{
    ev27_rx_frame_t fr;
    (void)argument;

    for (;;) {
        if (osMessageQueueGet(ev27_rx_queue, &fr, NULL, osWaitForever) != osOK) {
            continue;
        }
        /* TimeSync and Commands first; everything else is the device's. */
        if (!ev27_node_rx(fr.id, fr.data, fr.len, fr.time_us)) {
            ev27_device_rx(&fr);
        }
    }
}

void ev27_can_tx_task(void *argument)
{
    const ev27_node_info_t info = {
        .addr = EV27_MY_ADDR,
        .role = EV27_MY_ROLE,
        .fw_major = EV27_FW_MAJOR, .fw_minor = EV27_FW_MINOR, .fw_patch = EV27_FW_PATCH,
        .git_hash = EV27_GIT_HASH,
        .uid = {HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2()},
        .config_valid = ev27_device_config_valid(),
    };
    const uint32_t status_every = EV27_MAIN_STATUS_CYCLE_TIME_MS / EV27_TICK_MS;
    uint32_t tick = osKernelGetTickCount();
    uint32_t n = 0;
    (void)argument;

    ev27_node_init(&info);
    for (;;) {
        tick += EV27_TICK_MS;
        (void)osDelayUntil(tick);
        n++;

        ev27_can_service();
        ev27_device_tick(n);

        if (n % status_every == 0u) {
            ev27_device_status_t st = {0, 0, 0, 0};
            ev27_device_status(&st);
            ev27_node_set_sensors(st.sensors_enabled, st.sensors_present, st.sensors_healthy);
            ev27_node_set_error_flags(st.error_flags);
            ev27_node_send_status();   /* sent even while muted */
        }
    }
}
