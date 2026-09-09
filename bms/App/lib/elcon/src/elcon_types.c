#include "elcon_types.h"


elcon_internal_t g_elcon = {
    .max_voltage = 0,
    .max_current = 0,
    .charge_disable = 1,
    .heartbeat_msg = {0},
    .heartbeat_tick = 0,
};