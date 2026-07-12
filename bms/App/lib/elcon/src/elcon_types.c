#include "elcon_types.h"

elcon_internal_t hard_stop =
    (elcon_internal_t){.enable = 1, .max_current = 0, .max_voltage = 0};

elcon_internal_t g_elcon = {
    .max_voltage = 0,
    .max_current = 0,
    .enable = 1,
    .heartbeat_msg = {0},
    .heartbeat_tick = 0,
};