/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-07-23 03:10:36
 * @modify date 2025-07-23 03:23:33
 * @desc Current measurement source file
 */

#include "current.h"
#include "bms_driver.h"
#include "config.h"

volatile coulomb_counter_t g_state_of_charge = {0};

void current_measure() {
    // get the current draw from HC line
}

void coulomb_counter_init(coulomb_counter_t* soc) {
    if (!soc) {
        return;
    }
    soc->current = 0;
    soc->timestamp = 0;
    soc->accumulated_charge = 0;
    soc->error_callback = set_shutdown_circuit;
}
