# Debugging

In order to flash and debug the BMS firmware, we use `arm-none-eabi-gdb`
and `OpenOCD`.

We use the `Cortex-Debug` extension in VSCode for easy debugging with a
GUI interface.

## Debugging the BMS

To debug the STM32, an `ST-Link` debugger is required. `ST-Link` is hardware
debugging probe for debugging and programming STM32 devices over an SWD
interface.

NUCLEO development boards have one of these built-in, but debugging the BMS 
PCB will require an external `ST-Link` debugger.

The ST-Link will host a GDB server, which you can remotely connect to in a
GDB interface using `arm-none-eabi-gdb`. 

`arm-none-eabi-gdb` is a version of GDB that allows for remote debugging
bare-metal ARM devices like the STM32 microcontroller the BMS is running on.

This process will be facilitated
by `OpenOCD`, an interface that provides debugging, programming, and 
boundary-scan testing for embedded devices.

`OpenOCD` is used in a terminal interface. Cortex-Debug in VSCode is used 
for those who prefer a GUI environment.
