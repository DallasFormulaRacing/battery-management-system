#ifndef IMD_H
#define IMD_H

/*
 * Bender iso175C insulation monitor on the tb bus (FDCAN1, 500k, J1939).
 *
 * The IMD's status output (OKHS) and the latch that opens the shutdown circuit
 * are hardware. The BMS does not drive either. All it does here is:
 *   - read IMD_Info_General (every 100 ms) and keep the latest values,
 *   - check that the IMD's error and warning thresholds are what the car needs,
 *     write them if they are not, and read them back (the IMD does not
 *     acknowledge a write),
 *   - report the result as ImdConfig in BMS_State on the main bus.
 *
 * Messages are in ev27-can (can/vendor/tb/iso175c.dbc, Bender spec D00415).
 * Requests go out as IMD_Request 0x18EFF417 (BMS source address 0x17). The
 * answer's ID is not verified on the bench yet, so any PF 0xEF frame from the
 * IMD's address 0xF4 is taken as an answer.
 */

#include <stdbool.h>
#include <stdint.h>

/* Isolation below this faults the IMD (IsoAlarm, which drives OKHS).
 * Factory default is 100 kOhm. */
#define IMD_ERROR_THRESHOLD_KOHM 500U
/* Isolation below this sets IsoWarning only. Factory default is 500 kOhm. */
#define IMD_WARNING_THRESHOLD_KOHM 2000U

/* Same values as BMS_State.ImdConfig in ev27-can can/main/bms.yaml. */
typedef enum {
  IMD_CONFIG_NOT_SEEN = 0, /* no IMD_Info_General since boot */
  IMD_CONFIG_CHECKING = 1, /* reading (and if needed writing) the thresholds */
  IMD_CONFIG_OK = 2,       /* both thresholds read back as set above */
  IMD_CONFIG_FAILED = 3,   /* no answer, or still wrong after the writes */
  IMD_CONFIG_LOCKED = 4,   /* the IMD refused: its settings are locked */
  IMD_CONFIG_LOST = 5,     /* IMD_Info_General stopped for over a second */
} imd_config_state_t;

/* IMD_Info_General warnings and alarms, bit n = bit n of the Bender field. */
#define IMD_ALARM_DEVICE_ERROR (1U << 0)
#define IMD_ALARM_HV_POS_CONNECTION (1U << 1)
#define IMD_ALARM_HV_NEG_CONNECTION (1U << 2)
#define IMD_ALARM_EARTH_CONNECTION (1U << 3)
#define IMD_ALARM_ISO_ALARM (1U << 4)
#define IMD_ALARM_ISO_WARNING (1U << 5)
#define IMD_ALARM_ISO_OUTDATED (1U << 6)
#define IMD_ALARM_UNBALANCE (1U << 7)
#define IMD_ALARM_UNDERVOLTAGE (1U << 8)
#define IMD_ALARM_UNSAFE_TO_START (1U << 9)
#define IMD_ALARM_EARTHLIFT_OPEN (1U << 10)

#define IMD_R_ISO_NOT_VALID 65535U

typedef struct {
  bool online;                     /* Info_General seen within the last second */
  imd_config_state_t config;
  uint16_t r_iso_kohm;             /* R_iso_corrected, IMD_R_ISO_NOT_VALID = SNV */
  uint8_t r_iso_status;            /* Bender R_iso_status, 254 = normal operation */
  uint8_t device_activity;         /* 0 init, 1 normal, 2 self test */
  uint16_t alarms;                 /* IMD_ALARM_* bits */
  uint16_t error_threshold_kohm;   /* last value read back, 0 = not read */
  uint16_t warning_threshold_kohm; /* last value read back, 0 = not read */
  uint8_t last_error_code;         /* code of the last error answer, 0 = none */
  uint32_t info_count;             /* Info_General frames received */
} imd_status_t;

/* Sends one frame with a 29-bit ID on the tb bus. Returns true if queued. */
typedef bool (*imd_send_fn_t)(uint32_t ext_id, const uint8_t *data,
                              uint8_t len);

/* Call once before imd_rx() / imd_tick(). */
void imd_init(imd_send_fn_t send);

/* Every frame received on the tb bus. Ignores frames that are not the IMD's. */
void imd_rx(uint32_t id, bool extended, const uint8_t *data, uint8_t len,
            uint32_t now_ms);

/* Runs the threshold check. Call every 10 ms or so, from the same task as
 * imd_rx(). */
void imd_tick(uint32_t now_ms);

/* A copy of the latest status. Safe from any task. */
imd_status_t imd_get_status(void);

#endif /* IMD_H */
