/*
 * BMS watchdog: if the BMS state machine stops running for 20 s, open the
 * shutdown circuit, say so on the main bus and restart.
 *
 * bms_safety_task feeds it on every pass (every 200 ms), and so does the fault
 * loop in state.c, which never returns to that task. The HAL's 1 ms tick
 * interrupt (TIM5) calls bms_watchdog_tick_1ms(). Once 20 s pass without a
 * feed, that interrupt:
 *   1. pulls the SDC pin low (opens the shutdown circuit),
 *   2. sends one BMS_State frame on FDCAN2 with State = Fault,
 *      FaultCode = 14 (WatchdogRestart), FaultFlags bit 14 and
 *      ShutdownOpen = 1. It waits up to 5 ms for the frame to go out; with
 *      nothing on the bus to ACK it, it doesn't,
 *   3. restarts the MCU (NVIC_SystemReset).
 *
 * Only the state machine is watched. Missing hardware on the bench (segments,
 * IMD, charger, CM5) makes it fault, not stop, and the fault loop keeps
 * feeding, so a partly connected bench never trips the watchdog.
 *
 * Backstop for when interrupts stop too (hard fault, Error_Handler(), a stuck
 * interrupt): the independent watchdog (IWDG), set to about 25 s here. The
 * 1 ms tick reloads it until the 20 s timeout above trips. An IWDG reset sends
 * no CAN frame; the SDC pin has a hardware pull-down, so the reset opens the
 * shutdown circuit all the same.
 *
 * A debugger halting the core freezes both: the IWDG (DBGMCU) and the 20 s
 * count (the HAL tick stops). The ezfd bootloader starts the IWDG with 30 s
 * before it jumps to the app. An IWDG can't be stopped once started, so the
 * app sets it up in every build, with or without the bootloader.
 */
#ifndef BMS_WATCHDOG_H
#define BMS_WATCHDOG_H

/* No feed for this long: SDC low, CAN frame, restart. */
#define BMS_WDG_TIMEOUT_MS 20000U

/* IWDG backstop. LSI is 32 kHz nominal, 29.5-34 kHz over temperature, so this
 * fires after 23.5-27.1 s: always after the 20 s timeout. */
#define BMS_WDG_IWDG_TIMEOUT_MS 25000U

/* FaultCode in the last BMS_State frame: bms_fault_t BMS_ERR_WATCHDOG (state.c
 * checks they match), "WatchdogRestart" in ev27-can can/main/bms.yaml. */
#define BMS_WDG_FAULT_CODE 14U

/* main(), before the scheduler: starts the IWDG (or takes it over from the
 * bootloader) and starts the 20 s count. */
void bms_watchdog_start(void);

/* Every pass of the state machine (bms_safety_task and the fault loop). */
void bms_watchdog_feed(void);

/* HAL_TIM_PeriodElapsedCallback() for TIM5, the 1 ms HAL tick (main.c). */
void bms_watchdog_tick_1ms(void);

#endif /* BMS_WATCHDOG_H */
