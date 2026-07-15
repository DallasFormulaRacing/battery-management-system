#ifndef SOC_CURVE_H
#define SOC_CURVE_H

#include <stdint.h>
#include <stdbool.h>

#define DT_SECONDS 0.05f       // 50 ms time step
#define DEADZONE_THR 0.01f     // Current deadzone threshold (Amps)

#define SINGLE_18650_CHARGE_CAPACITY 3000
#define NUM_PARALLEL_CELLS 3
#define TOTAL_PACK_CAPACITY (SINGLE_18650_CHARGE_CAPACITY * NUM_PARALLEL_CELLS)

static batterymodel_t battery_model;
static emafilter_t soc_filter;
static const float q_total_pack = TOTAL_PACK_CAPACITY;

typedef struct {
    float pack_current;
    float min_cell_voltage;
    float max_cell_voltage;
    float accumulated_coulombs;
    float delta_charge_since_reset;
    float system_soc;
    uint32_t rest_duration_ms;
} batterymodel_t;

typedef struct {
    float alpha;
    float current_ema;
    bool is_initialized;
} emafilter_t;

#endif