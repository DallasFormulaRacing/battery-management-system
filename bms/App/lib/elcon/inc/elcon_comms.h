#ifndef ELCON_COMMS_H
#define ELCON_COMMS_H

#include "can.h"
#include "elcon_types.h"

/*
 * Elcon TC HK-L 6.6kW charger, standard CAN protocol No. 1000.
 * See docs/software/project/elcon_charger.md and
 * docs/software/project/dbc/elcon_tc_hk_l.dbc.
 *
 * Our charger runs at 500 kbit/s, Classic CAN, 29-bit extended IDs, DLC 8, big-endian.
 */
#define ELCON_CAN_BITRATE_BPS 500000U

/* Message 1: BMS -> charger, setpoints + control. Must be sent at least every
 * 1 s; after 5 s without it the charger stops with a comm fault (status bit4) */
#define ELCON_CHARGING_REQUEST_CAN_ID 0x1806E5F4U
/* Message 2: charger -> BMS broadcast, output readback + status, every 1 s */
#define ELCON_HEARTBEAT_CAN_ID 0x18FF50E5U

#define ELCON_FRAME_DLC 8U
#define ELCON_TX_PERIOD_MS 1000U
#define ELCON_RX_PERIOD_MS 1000U
#define ELCON_COMM_TIMEOUT_MS 5000U

/* request byte 4 (BYTE5 in the datasheet) */
#define ELCON_CONTROL_START 0U
#define ELCON_CONTROL_STOP 1U

/* broadcast bytes 2..3: bit 15 is the direction flag, bits 0..14 magnitude */
#define ELCON_CURRENT_DIR_BIT 0x8000U
#define ELCON_CURRENT_MAG_MASK 0x7FFFU

/* broadcast byte 4 status bits, 1 = fault */
#define ELCON_STATUS_HW_FAIL (1U << 0)
#define ELCON_STATUS_OVER_TEMP (1U << 1)
#define ELCON_STATUS_INPUT_VOLTAGE (1U << 2)
#define ELCON_STATUS_STARTING_STATE (1U << 3)
#define ELCON_STATUS_COMM_TIMEOUT (1U << 4)

/* hardcoded safety clamps applied to every outgoing request (whole V / A) */
#define ELCON_CLAMP_MAX_VOLTAGE_V LIMIT_CHARGER_VOLTAGE_MAX_V
#define ELCON_CLAMP_MAX_CURRENT_A LIMIT_CHARGER_CURRENT_MAX_A

void elcon_send_command(const elcon_internal_t *command_profile);

/* sends a stop request (0 V, 0 A, control = stop) */
void elcon_hard_stop(void);

can2_msg_t elcon_pack_can(const elcon_internal_t *command_profile);

elcon_status_t elcon_handle_heartbeat(const can2_msg_t *msg);

#endif
