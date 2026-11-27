#include "ev27_ezfd.h"

#include <stdbool.h>

#include "bms_board.h"
#include "ev27_node.h"
#include "ev27_port.h"
#include "fdcan.h"

#define EZFD_CMD_PING   0x0040u
#define EZFD_CMD_REBOOT 0x004Du
#define EZFD_PRIORITY   1u
/* Ping answer: [0 = bootloader / 1 = app, board]. The board byte (BMS_BOARD)
 * is the same tag as in the image header; the dashboard reads it from the
 * bootloader before it erases anything. */
#define EZFD_PING_APP   1u

static uint32_t ezfd_id(uint32_t prio, uint32_t target, uint32_t cmd, uint32_t src)
{
    return ((prio & 0x7u) << 26) | ((target & 0x1Fu) << 21) |
           ((cmd & 0xFFFFu) << 5) | (src & 0x1Fu);
}

void ev27_ezfd_rx(uint32_t ext_id, const uint8_t *data, uint8_t len)
{
    const uint32_t cmd = (ext_id >> 5) & 0xFFFFu;
    const uint32_t src = ext_id & 0x1Fu;

    (void)data;
    (void)len;

    switch (cmd) {
    case EZFD_CMD_PING: {
        const uint8_t reply[2] = {EZFD_PING_APP, BMS_BOARD};
        (void)ev27_port_send_frame(
            ezfd_id(EZFD_PRIORITY, src, EZFD_CMD_PING, FDCAN_EZFD_NODE_ADDR),
            true, reply, sizeof reply);
        break;
    }
    case EZFD_CMD_REBOOT:
        /* gated: VCU FlashAllowed (or the bench flag), no charging session */
        ev27_port_reboot_to_bootloader();
        break;
    default:
        break;
    }
}
