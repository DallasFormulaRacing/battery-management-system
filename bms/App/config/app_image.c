/*
 * Image header and end marker for the ezfd CAN bootloader.
 *
 * Only in the bootloader builds (Debug-BL / Release-BL): CMakeLists.txt adds
 * this file when BOOTLOADER is ON, and STM32G474XX_FLASH_BL.ld places
 *   .app_header  at 0x08008200, right after the vector table
 *   .app_trailer as the very last bytes of the image
 *
 * The bootloader only starts the app when
 *   - the header has the right magic, node (0x03, the BMS) and board (BMS_BOARD), and
 *   - the end marker the header points at is in flash. The dashboard writes the
 *     image front to back, so the marker is the last thing written: an upload
 *     that stopped half way leaves it erased and the app is not started.
 * The dashboard reads the same header from the .elf and refuses a file built
 * for another node or board before it erases anything.
 *
 * Header layout, little endian, 32 bytes. Keep in step with
 * bootloader/src/image.rs and the dashboard's ev27dash/firmware.py:
 *    0  u32  magic 0xFEEDFEED
 *    4  u8   header version, 1
 *    5  u8   ezfd node address (EV27_ADDR_BMS = 0x03)
 *    6  u8   board (BMS_BOARD_CONTROLLER = 1, BMS_BOARD_NUCLEO = 2)
 *    7  u8   0
 *    8  u32  address of the end marker
 *   12  u8   firmware version major, minor, patch (as in BMS_Status), then 0
 *   16  16 bytes 0, reserved
 * End marker: u32 0xBEEFBEEF, twice (one 8-byte flash double word).
 */
#include <stdint.h>

#include "bms_board.h"
#include "ev27_device_config.h"

#define APP_IMAGE_MAGIC      0xFEEDFEEDu
#define APP_IMAGE_END_MAGIC  0xBEEFBEEFu
#define APP_IMAGE_HEADER_VER 1u

typedef struct {
  uint32_t magic;
  uint8_t header_version;
  uint8_t node;
  uint8_t board;
  uint8_t reserved0;
  const uint32_t *end_marker;
  uint8_t fw_major;
  uint8_t fw_minor;
  uint8_t fw_patch;
  uint8_t reserved1;
  uint32_t reserved2[4];
} app_image_header_t;

_Static_assert(sizeof(app_image_header_t) == 32u, "ezfd image header must be 32 bytes");

__attribute__((section(".app_trailer"), used, aligned(8)))
const uint32_t app_image_end_marker[2] = {APP_IMAGE_END_MAGIC, APP_IMAGE_END_MAGIC};

__attribute__((section(".app_header"), used))
const app_image_header_t app_image_header = {
    .magic = APP_IMAGE_MAGIC,
    .header_version = APP_IMAGE_HEADER_VER,
    .node = EV27_MY_ADDR,
    .board = BMS_BOARD,
    .end_marker = app_image_end_marker,
    .fw_major = EV27_FW_MAJOR,
    .fw_minor = EV27_FW_MINOR,
    .fw_patch = EV27_FW_PATCH,
};
