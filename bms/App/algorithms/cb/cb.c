#include "cb.h"
#include "bms_types.h"

// ! table 95, page 68
// ! needs to be refactored properly, just messy rn for testing purposes

/**
 * @brief
 * Some things to think about:
 * how do we want to collect the cells that need to be balanced?
 * queue? all at once?
 *
 * collate all cell votlages from the asic array 192 element (asic) to 144
 * element (pcb) -- is this necessary? -- not really but could make life easier
 * we have a lot of memory to spare.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

/**
 * @brief cell balance super loop
 *
 *
 * @param asic_ctx:
 * @return void
 */
void cell_delta_policy_enforcer(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb) {
  // todo
};

void copy_cell_voltages(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb) {
  // copy cells from asic
}
