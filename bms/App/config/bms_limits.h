#ifndef BMS_LIMITS_H
#define BMS_LIMITS_H

/*
 * Single source of truth for the BMS hard limits.
 *
 * Only physical limits and absolute maximums live here: what the cells, the
 * pack and the charger can take. Protocol facts (CAN IDs, scaling, status
 * bits, timeouts the device dictates) stay with their driver.
 *
 * This header includes nothing on purpose so any layer can use it.
 */

/******************** Cell limits ********************/

/* any cell at/above this faults the BMS and counts as "pack full" */
#define LIMIT_CELL_OVERVOLTAGE_V 4.15F      // Cell is safe up to 4.2 V but that would cause a total pack voltage over 600V.
#define LIMIT_CELL_UNDERVOLTAGE_V 2.51F

#define LIMIT_CELL_OVERTEMP_C 60.0F
#define LIMIT_CELL_UNDERTEMP_C -20.0F

/******************** Pack charging limits ********************/

/* the most the BMS will ever ask the charger for, whatever the GUI sends */
#define LIMIT_PACK_CHARGE_VOLTAGE_MAX_V 600U
#define LIMIT_PACK_CHARGE_CURRENT_MAX_A 10U

/* smallest setpoint that counts as a real charge request */
#define LIMIT_PACK_CHARGE_CURRENT_MIN_A 1U

/******************** Charger hardware (Elcon HK-LF-540-14) ********************/

/* output range from the datasheet */
#define LIMIT_CHARGER_VOLTAGE_MIN_V 350U
#define LIMIT_CHARGER_VOLTAGE_MAX_V 680U
#define LIMIT_CHARGER_CURRENT_MAX_A 14U

/*
 * The charger is power limited, so it delivers less than 14 A at higher
 * voltages and roughly half the power on 110 VAC. It derates on its own; these
 * are here for reference and sanity checks.
 */
#define LIMIT_CHARGER_POWER_MAX_220VAC_W 6600U
#define LIMIT_CHARGER_POWER_MAX_110VAC_W 3300U

/******************** Consistency checks ********************/

_Static_assert(LIMIT_CELL_UNDERVOLTAGE_V < LIMIT_CELL_OVERVOLTAGE_V,
               "cell UV must be below cell OV");
_Static_assert(LIMIT_CELL_UNDERTEMP_C < LIMIT_CELL_OVERTEMP_C,
               "cell undertemp must be below overtemp");
_Static_assert(LIMIT_PACK_CHARGE_VOLTAGE_MAX_V <= LIMIT_CHARGER_VOLTAGE_MAX_V,
               "pack charge voltage above what the charger can output");
_Static_assert(LIMIT_PACK_CHARGE_VOLTAGE_MAX_V >= LIMIT_CHARGER_VOLTAGE_MIN_V,
               "pack charge voltage below the charger's output range");
_Static_assert(LIMIT_PACK_CHARGE_CURRENT_MAX_A <= LIMIT_CHARGER_CURRENT_MAX_A,
               "pack charge current above the charger rating");
_Static_assert(LIMIT_PACK_CHARGE_CURRENT_MIN_A <= LIMIT_PACK_CHARGE_CURRENT_MAX_A,
               "pack charge current min above max");

#endif
