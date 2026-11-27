#include "bms_watchdog.h"

#include <stdint.h>
#include <string.h>

#include "ev27_devices.h" /* EV27_MAIN_*_BITRATE */
#include "ev27_main.h"    /* BMS_State, from the ev27-can submodule */
#include "fdcan.h"        /* hfdcan2, the main bus */
#include "main.h"         /* SDC pin */
#include "stm32g4xx_hal.h"

/* IWDG key register values (RM0440 IWDG_KR). */
#define IWDG_KEY_RELOAD 0xAAAAu
#define IWDG_KEY_ENABLE 0xCCCCu
#define IWDG_KEY_ACCESS 0x5555u

/* LSI 32 kHz / 256 = 125 Hz: 8 ms per count. */
#define IWDG_PRESCALER_DIV256 6u
#define IWDG_RELOAD           ((BMS_WDG_IWDG_TIMEOUT_MS / 8U) - 1U)
_Static_assert(IWDG_RELOAD <= 0xFFFU, "BMS_WDG_IWDG_TIMEOUT_MS too long for the IWDG");
_Static_assert((uint64_t)BMS_WDG_IWDG_TIMEOUT_MS * 32000U / 34000U > BMS_WDG_TIMEOUT_MS + 1000U,
               "with a fast LSI the IWDG would reset before the 20 s timeout sends its frame");

/* Longest wait for the BMS_State frame to go out before the restart. */
#define TX_WAIT_MS 5U

_Static_assert(EV27_MAIN_BMS_STATE_LENGTH == 6U, "BMS_State length changed: update DataLength below");
_Static_assert(BMS_WDG_FAULT_CODE < 16U, "FaultFlags is 16 bits");
#ifdef EV27_MAIN_BMS_STATE_FAULT_CODE_WATCHDOG_RESTART_CHOICE /* ev27-can 0.7.0 and later */
_Static_assert(BMS_WDG_FAULT_CODE == EV27_MAIN_BMS_STATE_FAULT_CODE_WATCHDOG_RESTART_CHOICE,
               "FaultCode WatchdogRestart moved in ev27-can");
#endif

/* HAL tick at the last feed. */
static volatile uint32_t s_last_feed_ms;

void bms_watchdog_start(void)
{
  /* Stop the IWDG counter while a debugger has the core halted. */
  DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_IWDG_STOP;

  /* The tick interrupt reloads the IWDG, and a reload locks PR/RLR again, so
   * keep it out until the new values are in. */
  const uint32_t primask = __get_PRIMASK();
  __disable_irq();
  IWDG->KR = IWDG_KEY_ENABLE; /* starts it; no effect if the bootloader already did */
  IWDG->KR = IWDG_KEY_ACCESS;
  IWDG->PR = IWDG_PRESCALER_DIV256;
  IWDG->RLR = IWDG_RELOAD;
  /* PR/RLR take a few LSI cycles to reach the IWDG. Bounded so a dead LSI
   * can't hang the boot here. */
  for (uint32_t i = 0U; i < 2000000U && IWDG->SR != 0U; i++) {
  }
  IWDG->KR = IWDG_KEY_RELOAD;
  s_last_feed_ms = HAL_GetTick();
  __set_PRIMASK(primask);
}

void bms_watchdog_feed(void) { s_last_feed_ms = HAL_GetTick(); }

/* BMS_State with State = Fault, FaultCode = WatchdogRestart, ShutdownOpen = 1
 * (the other signals are 0). Waits up to TX_WAIT_MS for it to go out.
 *
 * This runs in the tick interrupt. A task can be half way through queuing a
 * frame of its own (fdcan2_tx() only locks the scheduler), and then one of the
 * two frames can be lost. Good enough for the last frame before a restart. */
static void send_restart_frame(void)
{
  struct ev27_main_bms_state_t m;
  uint8_t data[EV27_MAIN_BMS_STATE_LENGTH];
  FDCAN_TxHeaderTypeDef h;

  memset(&m, 0, sizeof(m));
  m.state = EV27_MAIN_BMS_STATE_STATE_FAULT_CHOICE;
  m.fault_code = BMS_WDG_FAULT_CODE;
  m.fault_flags = (uint16_t)(1U << BMS_WDG_FAULT_CODE);
  m.shutdown_open = 1U;
  if (ev27_main_bms_state_pack(data, &m, sizeof(data)) < 0) {
    return;
  }

  memset(&h, 0, sizeof(h));
  h.Identifier = EV27_MAIN_BMS_STATE_FRAME_ID;
  h.IdType = FDCAN_STANDARD_ID;
  h.TxFrameType = FDCAN_DATA_FRAME;
  h.DataLength = FDCAN_DLC_BYTES_6;
  h.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  h.BitRateSwitch = (EV27_MAIN_DATA_BITRATE != EV27_MAIN_NOMINAL_BITRATE) ? FDCAN_BRS_ON : FDCAN_BRS_OFF;
  h.FDFormat = FDCAN_FD_CAN;
  h.TxEventFifoControl = FDCAN_NO_TX_EVENTS;

  /* Time the wait with the cycle counter: HAL_GetTick() stands still in here. */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  const uint32_t start = DWT->CYCCNT;
  const uint32_t max_cycles = (SystemCoreClock / 1000U) * TX_WAIT_MS;

  while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan2) == 0U) {
    if ((DWT->CYCCNT - start) > max_cycles) {
      return; /* TX FIFO full: nobody is ACKing, or the bus is off */
    }
  }
  if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &h, data) != HAL_OK) {
    return;
  }
  const uint32_t buffer = HAL_FDCAN_GetLatestTxFifoQRequestBuffer(&hfdcan2);
  while (HAL_FDCAN_IsTxBufferMessagePending(&hfdcan2, buffer) != 0U &&
         (DWT->CYCCNT - start) <= max_cycles) {
  }
}

void bms_watchdog_tick_1ms(void)
{
  if ((HAL_GetTick() - s_last_feed_ms) < BMS_WDG_TIMEOUT_MS) {
    IWDG->KR = IWDG_KEY_RELOAD;
    return;
  }

  /* The state machine hasn't run for BMS_WDG_TIMEOUT_MS. */
  HAL_GPIO_WritePin(SDC_Pin_GPIO_Port, SDC_Pin_Pin, GPIO_PIN_RESET); /* open the shutdown circuit */
  send_restart_frame();
  NVIC_SystemReset();
}
