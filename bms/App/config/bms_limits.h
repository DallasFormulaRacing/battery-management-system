#ifndef BMS_LIMITS_H
#define BMS_LIMITS_H

/*
 * Single source of truth for the BMS hard limits.
 *
 * Only physical limits live here: what the cells, the pack and the charger can take.
 * Only use the LIMITS defined here, as these are the "official" numbers reported in ESF.
 * Additional constraints from FSAE rules force us to derate our setpoints from the physical limits.
 *  
 * Protocol details (CAN IDs, scaling, status bits, timeouts the device dictates) stay with their driver.
 *
 * This header includes nothing on purpose so any layer can use it.
 */

/******************** Cell limits ********************/

/* any cell at/above this faults the BMS and counts as "pack full" */
#define MAX_SAFE_CELL_VOLTAGE_V 4.20F
#define MIN_SAFE_CELL_VOLTAGE_V 2.50F
#define BMS_MEAS_VOLTAGE_ACCURACY_V 0.001F // +/- 1mV accuracy on cell voltage measurement
#define ESF_LEGAL_MAX_CELL_VOLTAGE_V 4.166F // Max allowable cell voltage to keep pack under 600V. (4.166V * 144 cells = 599.9V)
#define LIMIT_CELL_OVERVOLTAGE_V  (ESF_LEGAL_MAX_CELL_VOLTAGE_V - BMS_MEAS_VOLTAGE_ACCURACY_V) // We must account for worst case voltage accuracy
#define LIMIT_CELL_UNDERVOLTAGE_V  (MIN_SAFE_CELL_VOLTAGE_V + BMS_MEAS_VOLTAGE_ACCURACY_V)

#define MAX_SAFE_CELL_TEMP_C 60.0F
#define MIN_SAFE_CELL_TEMP_C -20.0F
#define BMS_MEAS_THERMISTOR_ACCURACY_DEG_C 1.0F
#define LIMIT_CELL_OVERTEMP_C  (MAX_SAFE_CELL_TEMP_C - BMS_MEAS_THERMISTOR_ACCURACY_DEG_C) // We must account for worst case thermistor accuracy
#define LIMIT_CELL_UNDERTEMP_C (MIN_SAFE_CELL_TEMP_C + BMS_MEAS_THERMISTOR_ACCURACY_DEG_C)

#define LIMIT_CELL_DISCHARGE_CURRENT_CONTINUOUS_A 60.0F // For 3P group, 60A per cell 
// #define LIMIT_CELL_DISCHARGE_CURRENT_PULSE_A 60.0F // Pulse TBD, dep on SoC, Temp, pulse time

#define LIMIT_CELL_CHARGE_CURRENT_FAST_A 25.0F // For 3P group, 25A per cell
// #define LIMIT_CELL_CHARGE_CURRENT_PULSE_A 25.0F // Pulse TBD, dep on SoC, Temp, pulse time

/******************** Pack limits ********************/
#define LIMIT_PACK_DISCHARGE_CURRENT_CONTINUOUS_A 180.0F // For 3P group, 60A per cell
#define LIMIT_PACK_CHARGE_CURRENT_FAST_A 75.0F // For 3P group, 25A per cell
// #define LIMIT_PACK_CHARGE_CURRENT_PULSE_A 75.0F // For 3P group, TBD per cell


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
