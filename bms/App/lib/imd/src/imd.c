/*
 * Bender iso175C: read Info_General, check and set the thresholds. See imd.h.
 *
 * Threshold check (runs after the IMD first appears, again whenever it comes
 * back after going quiet, and every 10 s while the result is not OK):
 *   for warning, then error (the warning threshold must stay above the error
 *   threshold, so it is raised first):
 *     GET; if the value differs, SET, wait 200 ms, GET again.
 *   A parameter gets at most IMD_MAX_WRITES writes per boot, so a setting that
 *   never sticks does not wear out the IMD's memory.
 *
 * Request format (Bender J1939 spec D00415, IMD_Request 0x18EFF417):
 *   get error threshold   46            -> answer 46 lo hi  (kOhm, little endian)
 *   set error threshold   47 lo hi
 *   get warning threshold 4A            -> answer 4A lo hi
 *   set warning threshold 4B lo hi
 *   refused               -> answer FF code index   (code 0x24 = settings locked)
 * Sets are not acknowledged. At most 10 requests per second.
 */
#include "imd.h"

#include <stddef.h>
#include <string.h>

#include "cmsis_os2.h"
#include "ev27_tb.h"

#define IMD_SOURCE_ADDRESS 0xF4U
#define J1939_PF_PROPRIETARY_A 0xEFU

#define IMD_INFO_TIMEOUT_MS 1000U  /* Info_General is every 100 ms */
#define IMD_SETTLE_MS 500U         /* after Info_General starts, before the first request */
#define IMD_REQUEST_GAP_MS 100U    /* at most 10 requests per second */
#define IMD_ANSWER_TIMEOUT_MS 300U /* answers normally come within 100 ms */
#define IMD_WRITE_SETTLE_MS 200U   /* after a set, before reading it back */
#define IMD_MAX_TRIES 5U           /* unanswered or refused requests per parameter per check */
#define IMD_MAX_WRITES 3U          /* writes per parameter per boot */
#define IMD_RETRY_MS 10000U        /* check again this long after FAILED or LOCKED */

#define IMD_ANSWER_ERROR 0xFFU
#define IMD_ERROR_LOCKED 0x24U

typedef struct {
  uint8_t get_index;
  uint8_t set_index;
  uint16_t target_kohm;
} imd_param_t;

static const imd_param_t k_params[] = {
    {0x4AU, 0x4BU, IMD_WARNING_THRESHOLD_KOHM},
    {0x46U, 0x47U, IMD_ERROR_THRESHOLD_KOHM},
};
#define IMD_PARAM_COUNT (sizeof(k_params) / sizeof(k_params[0]))

_Static_assert(IMD_ERROR_THRESHOLD_KOHM >= 30U && IMD_ERROR_THRESHOLD_KOHM <= 2000U,
               "iso175 error threshold range is 30..2000 kOhm");
_Static_assert(IMD_WARNING_THRESHOLD_KOHM >= 30U && IMD_WARNING_THRESHOLD_KOHM <= 2000U,
               "iso175 warning threshold range is 30..2000 kOhm");
_Static_assert(IMD_WARNING_THRESHOLD_KOHM >= IMD_ERROR_THRESHOLD_KOHM,
               "the warning threshold must not be below the error threshold");

typedef enum {
  STEP_WAIT_INFO, /* no Info_General (never, or not for a second) */
  STEP_SETTLE,    /* Info_General started, give the IMD a moment */
  STEP_GET,       /* send a get when the request gap allows */
  STEP_GET_WAIT,  /* wait for its answer */
  STEP_SET,       /* send a set when the request gap allows */
  STEP_SET_WAIT,  /* give the set time to take */
  STEP_DONE,      /* result in status.config */
} imd_step_t;

typedef struct {
  imd_send_fn_t send;
  imd_status_t st; /* private copy, published to s_pub */

  imd_step_t step;
  uint32_t step_ms; /* when the current step started */
  uint32_t last_info_ms;
  uint32_t last_request_ms;
  bool any_request;

  uint8_t param; /* index into k_params */
  uint8_t tries;
  uint8_t writes[IMD_PARAM_COUNT];

  /* Answer for the current parameter, filled in by imd_rx(). */
  bool answer_ready;
  bool answer_is_error;
  uint8_t answer_error_code;
  uint16_t answer_kohm;
} imd_ctx_t;

static imd_ctx_t s;
static imd_status_t s_pub;

static bool elapsed(uint32_t now_ms, uint32_t since_ms, uint32_t span_ms) {
  return (uint32_t)(now_ms - since_ms) >= span_ms;
}

static void publish(void) {
  const int32_t lock = osKernelLock();
  s_pub = s.st;
  (void)osKernelRestoreLock(lock);
}

static void go(imd_step_t step, uint32_t now_ms) {
  s.step = step;
  s.step_ms = now_ms;
}

static void finish(imd_config_state_t result, uint32_t now_ms) {
  s.st.config = result;
  go(STEP_DONE, now_ms);
}

static void start_check(uint32_t now_ms) {
  s.param = 0U;
  s.tries = 0U;
  s.answer_ready = false;
  s.st.config = IMD_CONFIG_CHECKING;
  go(STEP_GET, now_ms);
}

static bool request_gap_ok(uint32_t now_ms) {
  return !s.any_request || elapsed(now_ms, s.last_request_ms, IMD_REQUEST_GAP_MS);
}

static bool send_request(const uint8_t *data, uint8_t len, uint32_t now_ms) {
  s.last_request_ms = now_ms;
  s.any_request = true;
  return (s.send != NULL) && s.send(EV27_TB_IMD_REQUEST_FRAME_ID, data, len);
}

/* A get or set that went unanswered, was refused or could not be sent. */
static void request_failed(uint32_t now_ms) {
  s.tries++;
  if (s.tries >= IMD_MAX_TRIES) {
    finish(IMD_CONFIG_FAILED, now_ms);
  } else {
    go(STEP_GET, now_ms);
  }
}

static void store_readback(uint8_t param, uint16_t kohm) {
  if (k_params[param].get_index == 0x46U) {
    s.st.error_threshold_kohm = kohm;
  } else {
    s.st.warning_threshold_kohm = kohm;
  }
}

static void handle_info_general(const uint8_t *data, uint8_t len,
                                uint32_t now_ms) {
  struct ev27_tb_imd_info_general_t m;

  if (ev27_tb_imd_info_general_unpack(&m, data, len) != 0) {
    return;
  }
  s.st.r_iso_kohm = m.r_iso_corrected;
  s.st.r_iso_status = m.r_iso_status;
  s.st.device_activity = m.device_activity;
  s.st.alarms = (uint16_t)(
      ((m.device_error != 0U) ? IMD_ALARM_DEVICE_ERROR : 0U) |
      ((m.hv_pos_connection_failure != 0U) ? IMD_ALARM_HV_POS_CONNECTION : 0U) |
      ((m.hv_neg_connection_failure != 0U) ? IMD_ALARM_HV_NEG_CONNECTION : 0U) |
      ((m.earth_connection_failure != 0U) ? IMD_ALARM_EARTH_CONNECTION : 0U) |
      ((m.iso_alarm != 0U) ? IMD_ALARM_ISO_ALARM : 0U) |
      ((m.iso_warning != 0U) ? IMD_ALARM_ISO_WARNING : 0U) |
      ((m.iso_outdated != 0U) ? IMD_ALARM_ISO_OUTDATED : 0U) |
      ((m.unbalance_alarm != 0U) ? IMD_ALARM_UNBALANCE : 0U) |
      ((m.undervoltage_alarm != 0U) ? IMD_ALARM_UNDERVOLTAGE : 0U) |
      ((m.unsafe_to_start != 0U) ? IMD_ALARM_UNSAFE_TO_START : 0U) |
      ((m.earthlift_open != 0U) ? IMD_ALARM_EARTHLIFT_OPEN : 0U));
  s.st.info_count++;
  s.st.online = true;
  s.last_info_ms = now_ms;

  if (s.step == STEP_WAIT_INFO) {
    s.st.config = IMD_CONFIG_CHECKING;
    go(STEP_SETTLE, now_ms);
  }
}

static void handle_answer(const uint8_t *data, uint8_t len) {
  const imd_param_t *p;
  uint8_t index;

  if (len < 3U) {
    return;
  }
  if (data[0] == IMD_ANSWER_ERROR) {
    s.st.last_error_code = data[1];
    index = data[2];
  } else {
    index = data[0];
  }
  if (s.param >= IMD_PARAM_COUNT) {
    return;
  }
  p = &k_params[s.param];
  if (index != p->get_index && index != p->set_index) {
    return; /* not about the parameter being checked */
  }
  s.answer_ready = true;
  s.answer_is_error = (data[0] == IMD_ANSWER_ERROR);
  s.answer_error_code = s.answer_is_error ? data[1] : 0U;
  s.answer_kohm = (uint16_t)((uint16_t)data[1] | ((uint16_t)data[2] << 8));
}

void imd_init(imd_send_fn_t send) {
  memset(&s, 0, sizeof(s));
  s.send = send;
  s.st.config = IMD_CONFIG_NOT_SEEN;
  s.st.r_iso_kohm = IMD_R_ISO_NOT_VALID;
  s.step = STEP_WAIT_INFO;
  publish();
}

void imd_rx(uint32_t id, bool extended, const uint8_t *data, uint8_t len,
            uint32_t now_ms) {
  if (!extended || data == NULL) {
    return;
  }
  if (id == EV27_TB_IMD_INFO_GENERAL_FRAME_ID) {
    handle_info_general(data, len, now_ms);
  } else if ((id & 0xFFU) == IMD_SOURCE_ADDRESS &&
             ((id >> 16) & 0xFFU) == J1939_PF_PROPRIETARY_A) {
    handle_answer(data, len);
  } else {
    return;
  }
  publish();
}

void imd_tick(uint32_t now_ms) {
  const imd_param_t *p = &k_params[(s.param < IMD_PARAM_COUNT) ? s.param : 0U];

  if (s.st.online && elapsed(now_ms, s.last_info_ms, IMD_INFO_TIMEOUT_MS)) {
    s.st.online = false;
    s.st.config = IMD_CONFIG_LOST;
    go(STEP_WAIT_INFO, now_ms);
  }

  switch (s.step) {
  case STEP_WAIT_INFO:
    break;

  case STEP_SETTLE:
    if (elapsed(now_ms, s.step_ms, IMD_SETTLE_MS)) {
      start_check(now_ms);
    }
    break;

  case STEP_GET:
    if (request_gap_ok(now_ms)) {
      const uint8_t req[1] = {p->get_index};
      s.answer_ready = false;
      if (send_request(req, sizeof(req), now_ms)) {
        go(STEP_GET_WAIT, now_ms);
      } else {
        request_failed(now_ms);
      }
    }
    break;

  case STEP_GET_WAIT:
    if (s.answer_ready) {
      s.answer_ready = false;
      if (s.answer_is_error) {
        if (s.answer_error_code == IMD_ERROR_LOCKED) {
          finish(IMD_CONFIG_LOCKED, now_ms);
        } else {
          request_failed(now_ms);
        }
      } else if (s.answer_kohm == p->target_kohm) {
        store_readback(s.param, s.answer_kohm);
        s.param++;
        s.tries = 0U;
        if (s.param >= IMD_PARAM_COUNT) {
          finish(IMD_CONFIG_OK, now_ms);
        } else {
          go(STEP_GET, now_ms);
        }
      } else {
        store_readback(s.param, s.answer_kohm);
        if (s.writes[s.param] >= IMD_MAX_WRITES) {
          finish(IMD_CONFIG_FAILED, now_ms);
        } else {
          go(STEP_SET, now_ms);
        }
      }
    } else if (elapsed(now_ms, s.step_ms, IMD_ANSWER_TIMEOUT_MS)) {
      request_failed(now_ms);
    }
    break;

  case STEP_SET:
    if (request_gap_ok(now_ms)) {
      const uint8_t req[3] = {p->set_index, (uint8_t)(p->target_kohm & 0xFFU),
                              (uint8_t)(p->target_kohm >> 8)};
      s.answer_ready = false;
      s.writes[s.param]++;
      if (send_request(req, sizeof(req), now_ms)) {
        go(STEP_SET_WAIT, now_ms);
      } else {
        request_failed(now_ms);
      }
    }
    break;

  case STEP_SET_WAIT:
    if (s.answer_ready && s.answer_is_error) {
      s.answer_ready = false;
      if (s.answer_error_code == IMD_ERROR_LOCKED) {
        finish(IMD_CONFIG_LOCKED, now_ms);
      } else {
        request_failed(now_ms);
      }
    } else if (elapsed(now_ms, s.step_ms, IMD_WRITE_SETTLE_MS)) {
      go(STEP_GET, now_ms);
    }
    break;

  case STEP_DONE:
  default:
    if (s.st.config != IMD_CONFIG_OK &&
        elapsed(now_ms, s.step_ms, IMD_RETRY_MS)) {
      start_check(now_ms);
    }
    break;
  }
  publish();
}

imd_status_t imd_get_status(void) {
  imd_status_t copy;
  const int32_t lock = osKernelLock();
  copy = s_pub;
  (void)osKernelRestoreLock(lock);
  return copy;
}
