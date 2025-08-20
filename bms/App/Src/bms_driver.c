/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-04 15:19:12
 * @modify date 2025-08-04 15:19:12
 * @desc AD BMS 6830 driver definition file
 */

#include "bms_driver.h"

SPI_HandleTypeDef hspi2; // SPI handle for AD BMS 6830

void bms_wakeup() {
// spi_send_dummy_bytes();                     // Step 1: Wake up pulse
// delay_ms(5);                                // Give it time
// send_command(RDCFGA);                       // Step 2: Ask for config
// if (check_valid_response()) {
//     return WAKE_SUCCESS;
// } else {
//     return WAKE_FAILED;
// }


}