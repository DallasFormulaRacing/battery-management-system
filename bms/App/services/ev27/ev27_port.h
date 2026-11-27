/* Shared between ev27_port_stm32.c, ev27_can_tasks.c and ev27_device.c (from the ev27-can template). */
#ifndef EV27_PORT_H
#define EV27_PORT_H

#include <stdbool.h>
#include <stdint.h>

#include "cmsis_os2.h"

typedef struct {
    uint32_t id;
    uint32_t time_us;   /* ev27_port_time_us() when the frame arrived */
    uint8_t  len;
    uint8_t  data[64];
} ev27_rx_frame_t;

/* What this device reports in Status. Bit meanings are in can/main/status.yaml. */
typedef struct {
    uint16_t sensors_enabled;
    uint16_t sensors_present;
    uint16_t sensors_healthy;
    uint16_t error_flags;
} ev27_device_status_t;

/* Period of the TX task, and so of ev27_device_tick(). */
#define EV27_TICK_MS 10u

extern osMessageQueueId_t ev27_rx_queue;

/* Task attributes, for osThreadNew() in main.c. */
extern const osThreadAttr_t ev27_can_rx_task_attributes;
extern const osThreadAttr_t ev27_can_tx_task_attributes;

/* ---- Generic, in ev27_port_stm32.c and ev27_can_tasks.c ------------------- */

/* Sets up FDCAN2 (ev27 11-bit filters, the GUI's 29-bit filter, reject the
 * rest), creates ev27_rx_queue and starts FDCAN2. Call once after
 * osKernelInitialize(), before the scheduler starts. */
void ev27_can_start(void);

/* From the FDCAN2 RX interrupt (HAL_FDCAN_RxFifo0Callback in gui_can_job.c):
 * queue one 11-bit main-bus frame for the RX task. time_us is
 * ev27_port_time_us() read at the start of the interrupt. */
void ev27_can_queue_rx_from_isr(uint32_t id, const uint8_t *data, uint8_t len, uint32_t time_us);

/* Queue one CAN FD frame on the main bus (no BRS while the bus runs 1M/1M).
 * ev27_port_send() is this with extended = false. The BMS also uses it to
 * forward tb-bus frames with their own 29-bit IDs (can2_job.c). It does not
 * check the CM5's mute: callers that send data messages must check
 * ev27_node_muted() first. Returns 0 if queued. */
int ev27_port_send_frame(uint32_t id, bool extended, const uint8_t *data, uint8_t len);

/* Called every tick by the TX task: restarts FDCAN2 after a bus-off. */
void ev27_can_service(void);

bool ev27_identify_active(void);

void ev27_can_rx_task(void *argument);
void ev27_can_tx_task(void *argument);

/* ---- Device-specific, in ev27_device.c -------------------------------------
 * The generic tasks call these. They are the only place the BMS's own
 * messages appear, so ev27_can_tasks.c never names a BMS message. */

/* Every received main-bus frame that ev27_node did not handle (anything other
 * than TimeSync and a Command for this node). Runs in the RX task. */
void ev27_device_rx(const ev27_rx_frame_t *fr);

/* Every EV27_TICK_MS from the TX task. n counts ticks from 1. */
void ev27_device_tick(uint32_t n);

/* Fill in what goes into this device's next Status (every 500 ms). */
void ev27_device_status(ev27_device_status_t *st);

/* True if the device's config record in flash passed its CRC check. The BMS
 * has no config record yet: always true. */
bool ev27_device_config_valid(void);

/* True if a reboot request (ev27 Command RebootToBootloader or ezfd Reboot)
 * may reset the BMS now: no charging session, and the VCU says FlashAllowed
 * (or BMS_BENCH_REBOOT_WITHOUT_VCU). Allowed in Fault: the shutdown-circuit
 * latch is hardware, so a reset cannot clear a latched AMS fault. */
bool ev27_device_reboot_allowed(void);

#endif /* EV27_PORT_H */
