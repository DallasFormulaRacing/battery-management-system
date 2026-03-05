#ifndef BMS_CAN_PROTOCOL_H
#define BMS_CAN_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

/* =========================================================
   29-bit Extended CAN ID layout
   [28:26] priority (3)
   [25:21] target   (5)
   [20:5]  cmd      (16)
   [4:0]   source   (5)
   ========================================================= */

typedef uint32_t can_ext_id_t;

enum
{
    CAN_EXT_ID_MASK = 0x1FFFFFFFU
};

/* ---- Stronger types for each field ---- */

typedef enum
{
    CAN_PRIORITY_CRITICAL  = 0x0,
    CAN_PRIORITY_VERY_HIGH = 0x1,
    CAN_PRIORITY_HIGH      = 0x2,
    CAN_PRIORITY_MEDIUM    = 0x3,
    CAN_PRIORITY_LOW       = 0x4,
    CAN_PRIORITY_VERY_LOW  = 0x5
} can_priority_t;

typedef enum
{
    BMS_DEVICE_ID = 0x1F,
    GUI_DEVICE_ID = 0x1E
} can_device_id_t;

typedef enum
{
    CMD_ID_SVOLTAGE_ALL      = 0x00A0,
    CMD_ID_SVOLTAGE_ALL_RESP = 0x00A1,
    CMD_ID_CVOLTAGE_ALL      = 0x00A2,
    CMD_ID_CVOLTAGE_ALL_RESP = 0x00A3
} can_command_id_t;

/* =========================================================
   Helpers (type-safe, debugger-friendly)
   ========================================================= */

static inline can_ext_id_t can_id_build(can_priority_t priority,
                                       can_device_id_t target,
                                       can_command_id_t cmd,
                                       can_device_id_t source)
{
    /* mask each field to its width so out-of-range values can’t corrupt other fields */
    uint32_t p = ((uint32_t)priority & 0x07U) << 26;
    uint32_t t = ((uint32_t)target   & 0x1FU) << 21;
    uint32_t c = ((uint32_t)cmd      & 0xFFFFU) << 5;
    uint32_t s = ((uint32_t)source   & 0x1FU);

    return (can_ext_id_t)((p | t | c | s) & CAN_EXT_ID_MASK);
}

static inline can_priority_t can_id_get_priority(can_ext_id_t id)
{
    return (can_priority_t)((id >> 26) & 0x07U);
}

static inline can_device_id_t can_id_get_target(can_ext_id_t id)
{
    return (can_device_id_t)((id >> 21) & 0x1FU);
}

static inline can_command_id_t can_id_get_cmd(can_ext_id_t id)
{
    return (can_command_id_t)((id >> 5) & 0xFFFFU);
}

static inline can_device_id_t can_id_get_source(can_ext_id_t id)
{
    return (can_device_id_t)(id & 0x1FU);
}

static inline bool can_id_is_valid(can_ext_id_t id)
{
    // Fits in 29 bits
    return (id & ~CAN_EXT_ID_MASK) == 0U;
}

/* =========================================================
   Convenience constructors for current message IDs
   ========================================================= */

static inline can_ext_id_t can_id_read_svoltage_all(void)
{
    return can_id_build(CAN_PRIORITY_MEDIUM, BMS_DEVICE_ID,
                        CMD_ID_SVOLTAGE_ALL, GUI_DEVICE_ID);
}

static inline can_ext_id_t can_id_read_svoltage_all_resp(void)
{
    return can_id_build(CAN_PRIORITY_MEDIUM, GUI_DEVICE_ID,
                        CMD_ID_SVOLTAGE_ALL_RESP, BMS_DEVICE_ID);
}

static inline can_ext_id_t can_id_read_cvoltage_all(void)
{
    return can_id_build(CAN_PRIORITY_MEDIUM, BMS_DEVICE_ID,
                        CMD_ID_CVOLTAGE_ALL, GUI_DEVICE_ID);
}

static inline can_ext_id_t can_id_read_cvoltage_all_resp(void)
{
    return can_id_build(CAN_PRIORITY_MEDIUM, GUI_DEVICE_ID,
                        CMD_ID_CVOLTAGE_ALL_RESP, BMS_DEVICE_ID);
}

#endif 