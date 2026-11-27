#ifndef EV27_EZFD_H
#define EV27_EZFD_H

#include <stdint.h>

/*
 * The app's side of the ezfd bootloader protocol, so the dashboard's Ping,
 * Reboot and Flash buttons work while the BMS app runs (the bootloader
 * answers them itself when it runs). 29-bit IDs:
 * [prio 3][target 5][command 16][source 5], target = FDCAN_EZFD_NODE_ADDR.
 *
 *   Ping   (0x0040): answered with one byte, 1 = app (the bootloader says 0).
 *   Reboot (0x004D): NVIC_SystemReset() if ev27_device_reboot_allowed().
 *
 * Anything else addressed to the BMS's node address is ignored.
 * Task context (gui_can_job_runner).
 */
void ev27_ezfd_rx(uint32_t ext_id, const uint8_t *data, uint8_t len);

#endif /* EV27_EZFD_H */
