/* STM32G474RE: the bootloader owns the first 32 KB of flash. The BMS app is
 * linked at 0x08008000 (bms/STM32G474XX_FLASH_BL.ld). */
MEMORY
{
  FLASH : ORIGIN = 0x08000000, LENGTH = 32K
  RAM   : ORIGIN = 0x20000000, LENGTH = 128K
}
