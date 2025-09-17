/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-07-23 03:09:43
 * @modify date 2025-07-23 03:15:06
 * @desc Current measurement header file
 */

#ifndef CURRENT_H
#define CURRENT_H

#include "config.h"
#include <stdint.h>

typedef struct {
    uint32_t current;        // Current measurement in microamperes
    uint32_t timestamp;      // Timestamp of the measurement
    uint32_t accumulated_charge; // Accumulated charge in coulombs
    void (*error_callback)(SHUTDOWN_CIRCUIT_STATE); // Overcurrent -> open shutdown circuit
} coulomb_counter_t;

void start_current_measurement(void);

void stop_current_measurement(void);

void coulomb_counter(void);

void coulomb_counter_init(coulomb_counter_t *soc);

void current_measure();



#endif // CURRENT_H