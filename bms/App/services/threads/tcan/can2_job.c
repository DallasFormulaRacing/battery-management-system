#include "can2_job.h"
#include "can.h"
#include "cmsis_os2.h"
#include "elcon_comms.h"
#include "fdcan.h"
#include "gui_types.h"
#include "imd_drivers.h"
#include "imd_types.h"
#include "stm32g474xx.h"
#include <string.h>

// make sure this exists irl
extern osMessageQueueId_t can2_rx_dispatch_queueHandle;

static uint32_t can2_len_to_dlc(uint32_t len);
static HAL_StatusTypeDef forward_can2_to_fdvcan(const can2_msg_t *msg);
static void process_can2_protocols(const can2_msg_t *msg);
static void handle_forwarding(const can2_msg_t *msg);

static volatile can2_forwarding_stats_t g_can2_forwarding_stats = {0};

static uint32_t can2_len_to_dlc(uint32_t len) {
  static const uint32_t dlc_codes[9] = {
      FDCAN_DLC_BYTES_0, FDCAN_DLC_BYTES_1, FDCAN_DLC_BYTES_2,
      FDCAN_DLC_BYTES_3, FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5,
      FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7, FDCAN_DLC_BYTES_8,
  };

  if (len > 8U) {
    return FDCAN_DLC_BYTES_8;
  }
  return dlc_codes[len];
}

static HAL_StatusTypeDef forward_can2_to_fdvcan(const can2_msg_t *msg) {
  FDCAN_TxHeaderTypeDef header = {
      .IdType = FDCAN_EXTENDED_ID,
      .Identifier = 0,
      .TxFrameType = FDCAN_DATA_FRAME,
      .DataLength = can2_len_to_dlc(msg->len),
      .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
      .BitRateSwitch = FDCAN_BRS_ON,
      .FDFormat = FDCAN_FD_CAN,
      .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
      .MessageMarker = 0U,
  };

  switch (msg->id) {
  case PLACEHOLDER_CURRENT_SENSOR_CAN_ID:
    header.Identifier = can_id_build(CAN_PRIORITY_P1, GUI_DEVICE_ID,
                                     DFR_CAN_BMS_CURRENT_SENSOR, BMS_DEVICE_ID);
    break;
  case IMD_CAN_ID_REQUEST:
    header.Identifier = can_id_build(CAN_PRIORITY_P1, GUI_DEVICE_ID,
                                     DFR_CAN_BMS_IMD_REQUEST, BMS_DEVICE_ID);
    break;
  case IMD_CAN_ID_RESPONSE:
    header.Identifier = can_id_build(CAN_PRIORITY_P1, GUI_DEVICE_ID,
                                     DFR_CAN_BMS_IMD_RESPONSE, BMS_DEVICE_ID);
    break;
  case IMD_CAN_ID_GENERAL:
    header.Identifier = can_id_build(CAN_PRIORITY_P1, GUI_DEVICE_ID,
                                     DFR_CAN_BMS_IMD_GENERAL, BMS_DEVICE_ID);
    break;
  case IMD_CAN_ID_ISO_DETAIL:
    header.Identifier = can_id_build(CAN_PRIORITY_P1, GUI_DEVICE_ID,
                                     DFR_CAN_BMS_IMD_ISO_DETAIL, BMS_DEVICE_ID);
    break;
  case IMD_CAN_ID_VOLTAGE:
    header.Identifier = can_id_build(CAN_PRIORITY_P1, GUI_DEVICE_ID,
                                     DFR_CAN_BMS_IMD_VOLTAGE, BMS_DEVICE_ID);
    break;
  case IMD_CAN_ID_IT_SYSTEM:
    header.Identifier = can_id_build(CAN_PRIORITY_P1, GUI_DEVICE_ID,
                                     DFR_CAN_BMS_IMD_IT_SYSTEM, BMS_DEVICE_ID);
    break;
  default:
    break;
  }

  // todo map can ids to dfr
  return fdcan_send(header.Identifier, msg->data, header.DataLength);
}

static void process_can2_protocols(const can2_msg_t *msg) {
  if (msg->id == IMD_CAN_ID_REQUEST) {
    parse_imd_msg(msg->id, msg->data);
  }

  if (msg->id == ELCON_HEARTBEAT_CAN_ID) {
    elcon_handle_heartbeat(msg);
  }

  // if (msg->id == CAB_CAN_ID_REQUEST 1) {
  //   parse_current_sensor_msg(msg.id, msg.data);
  // }
}

static void handle_forwarding(const can2_msg_t *msg) {
  g_can2_forwarding_stats.rx_total++;

  if (forward_can2_to_fdvcan(msg) == HAL_OK) {
    g_can2_forwarding_stats.forwarded_total++;
  } else {
    g_can2_forwarding_stats.forward_drop_total++;
  }
}

void can2_forwarding_count_queue_overrun(void) {
  g_can2_forwarding_stats.queue_overrun_total++;
}

can2_forwarding_stats_t can2_forwarding_stats_get(void) {
  return (can2_forwarding_stats_t)g_can2_forwarding_stats;
}

const osThreadAttr_t can2_job_runner_attributes = {
    .name = "can2_job_runner",
    .priority = (osPriority_t)osPriorityHigh,
    .stack_size = 128 * 8};

void can2_job_runner(void *argument) {
  (void)argument;
  can2_msg_t msg;
  can2_hardware_init();

  for (;;) {
    if (osMessageQueueGet(can2_rx_dispatch_queueHandle, &msg, NULL,
                          osWaitForever) != osOK) {
      continue;
    }

    handle_forwarding(&msg);
    process_can2_protocols(&msg);
  }
}
