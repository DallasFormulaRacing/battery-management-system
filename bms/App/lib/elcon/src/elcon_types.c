#include "elcon_types.h"


elcon_internal_t g_elcon = {
    .max_voltage_dv = 0,
    .max_current_da = 0,
    .charge_disable = 1,
    .heartbeat_msg = {0},
    .heartbeat_tick = 0,
    .output_voltage_dv = 0,
    .output_current_da = 0,
    .output_discharging = 0,
};