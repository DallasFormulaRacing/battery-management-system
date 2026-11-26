/*
 * tb bus (FDCAN1, 500k classic CAN) task.
 *
 * Every received frame:
 *   - is copied to the main bus if ev27-can lists it in EV27_FORWARD_TB_TO_MAIN
 *     (can/main/forward_tb.yaml). The copy keeps the tb ID (11- or 29-bit) and
 *     the payload, so a frame looks the same on both buses.
 *   - goes to the IMD and Elcon handlers.
 * Between frames (at least every 10 ms) it runs the IMD threshold check and
 * the FDCAN1 bus-off recovery.
 */
#include "can2_job.h"
#include "can.h"
#include "cmsis_os2.h"
#include "elcon_comms.h"
#include "ev27_forward_main.h"
#include "ev27_node.h"
#include "ev27_port.h"
#include "imd.h"
#include <stdbool.h>

extern osMessageQueueId_t can2_rx_dispatch_queueHandle;

#define CAN2_IDLE_TICK_MS 10U

static volatile can2_stats_t g_can2_stats = {0};
static uint32_t s_forward_last_ms[EV27_FORWARD_TB_TO_MAIN_COUNT];
static bool s_forward_sent[EV27_FORWARD_TB_TO_MAIN_COUNT];

static bool imd_send(uint32_t ext_id, const uint8_t *data, uint8_t len) {
  return can2_send_ext(ext_id, data, len) == HAL_OK;
}

static void forward_to_main(const can2_msg_t *msg, uint32_t now_ms) {
  const bool extended = (msg->id_type == FDCAN_EXTENDED_ID);

  for (uint32_t i = 0U; i < EV27_FORWARD_TB_TO_MAIN_COUNT; i++) {
    const ev27_forward_t *f = &EV27_FORWARD_TB_TO_MAIN[i];

    if (f->src_id != msg->id || (f->src_extended != 0U) != extended) {
      continue;
    }
    if (f->min_period_ms > 0U && s_forward_sent[i] &&
        (uint32_t)(now_ms - s_forward_last_ms[i]) < f->min_period_ms) {
      return;
    }
    /* A muted node sends Status only (ev27-can CONVENTIONS.md). */
    if (ev27_node_muted()) {
      g_can2_stats.forward_muted_total++;
      return;
    }
    if (ev27_port_send_frame(f->dst_id, f->dst_extended != 0U, msg->data,
                             (uint8_t)msg->len) == 0) {
      s_forward_last_ms[i] = now_ms;
      s_forward_sent[i] = true;
      g_can2_stats.forwarded_total++;
    } else {
      g_can2_stats.forward_drop_total++;
    }
    return;
  }
}

static void process_can2_protocols(const can2_msg_t *msg, uint32_t now_ms) {
  imd_rx(msg->id, msg->id_type == FDCAN_EXTENDED_ID, msg->data,
         (uint8_t)msg->len, now_ms);

  if (msg->id == ELCON_HEARTBEAT_CAN_ID) {
    elcon_handle_heartbeat(msg);
  }
}

void can2_count_queue_overrun(void) { g_can2_stats.queue_overrun_total++; }

can2_stats_t can2_stats_get(void) { return (can2_stats_t)g_can2_stats; }

const osThreadAttr_t can2_job_runner_attributes = {
    .name = "can2_job_runner",
    .priority = (osPriority_t)osPriorityHigh,
    .stack_size = 128 * 8};

void can2_job_runner(void *argument) {
  (void)argument;
  can2_msg_t msg;

  imd_init(imd_send);
  can2_hardware_init();

  for (;;) {
    if (osMessageQueueGet(can2_rx_dispatch_queueHandle, &msg, NULL,
                          CAN2_IDLE_TICK_MS) == osOK) {
      const uint32_t now_ms = osKernelGetTickCount();
      g_can2_stats.rx_total++;
      forward_to_main(&msg, now_ms);
      process_can2_protocols(&msg, now_ms);
    }

    imd_tick(osKernelGetTickCount());
    can2_service();
  }
}
