# Real-time operating system for BMS firmware

### Important Links

- [CMSIS Documentation](<[text](https://arm-software.github.io/CMSIS_5/RTOS2/html/)>)

- [CMSIS-FreeRTOS Documentation](https://arm-software.github.io/CMSIS-FreeRTOS/main/index.html)

We will use CMSIS-FreeRTOS, the CMSIS adaptation of FreeRTOS based off the version 11.2 kernel.

To clarify, CMSIS-RTOS is a software interface developed to wrap around an actual, concrete RTOS, in our case, FreeRTOS. FreeRTOS provides the core functionalities of an RTOS, such as task scheduling, inter-task communication (queues, semaphores, mutexes), and memory management.

In a way, the CMSIS RTOS can be thought of as an abstraction layer, though not necessarily a _hardware_ abstraction layer (HAL).

## 1. Introduction

### Theory of Operation:

In order to balance and schedule the various functions and execute their capabilities, the standard while loop is no longer sufficient. Therefore, we need to implement a scheduler to queue threads of execution to handle the many functions of the BMS Host MCU.

Many aspects of the kernel are configurable and the configuration options are mentioned where applicable.

### `Systick`

The systick timer is a 24-bit countdown timer with an auto reload. It is generally used to provide a periodic interrupt for an RTOS scheduler.

### System Startup Theory

Once the execution reaches `main()` there is a recommended order to initialize the hardware and start the kernel.

Your application's `main()` should implement at least the following in the given order:

- Initialization and configuration of hardware including peripherals, memory, pins, clocks and the interrupt system.
- Update the system core clock using the respective CMSIS-Core (Cortex-M) or CMSIS-Core (Cortex-A) function.
- Initialize the CMSIS-RTOS kernel using `osKernelInitialize`.
- Optionally, create one thread (for example app_main), which is used as a main thread using `osThreadNew`. This thread should take care of creating and starting objects, once it is run by the scheduler. Alternatively, threads can be created in `main()` directly.
- Start the RTOS scheduler using `osKernelStart` which also configures the system tick timer and initializes RTOS specific interrupts.
- This function does not return in case of successful execution. Therefore, any application code after `osKernelStart` will not be executed.
- Meaning that `osKernelStart` should be the last function called in `main()`.
