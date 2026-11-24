/*
 * STM32 FDCAN HAL + CMSIS-RTOS2 glue for the main bus (from the ev27-can
 * template, adapted for the BMS).
 *
 * Differences from the template, because FDCAN2 also carries the GUI's 29-bit
 * request/response protocol (lib/gui):
 *   - No HAL_FDCAN_RxFifo0Callback here. The one in services/threads/vcan/
 *     gui_can_job.c sorts FDCAN2 frames: 11-bit -> ev27_can_queue_rx_from_isr(),
 *     29-bit -> the GUI queue.
 *   - ev27_can_start() also sets up the GUI's 29-bit filter, through
 *     fdcan_hardware_init().
 *   - Frames are queued through fdcan2_tx(), which the GUI uses too, so the two
 *     never write the TX FIFO at the same time.
 */
#include <string.h>

#include "cmsis_os2.h"
#include "fdcan.h"

#include "ev27_device_config.h"
#include "ev27_node.h"
#include "ev27_port.h"

#define EV27_RX_QUEUE_LEN 16u

osMessageQueueId_t ev27_rx_queue;
static volatile uint32_t s_identify_until_ms;

/* CAN FD DLC codes 0-15 for each payload length. */
static uint32_t len_to_dlc(uint8_t len)
{
    static const uint8_t sizes[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};
    uint32_t dlc;
    for (dlc = 0; dlc < 15u; dlc++) {
        if (sizes[dlc] >= len) {
            break;
        }
    }
    return dlc;   /* FDCAN_DLC_BYTES_x on the G4 HAL are these same codes */
}

void ev27_can_start(void)
{
    static const uint32_t ids[] = EV27_RX_IDS;
    const uint32_t n = sizeof(ids) / sizeof(ids[0]);
    FDCAN_FilterTypeDef f;
    uint32_t i;

    ev27_rx_queue = osMessageQueueNew(EV27_RX_QUEUE_LEN, sizeof(ev27_rx_frame_t), NULL);

    /* Two IDs per standard filter. */
    for (i = 0; i < n; i += 2) {
        memset(&f, 0, sizeof(f));
        f.IdType = FDCAN_STANDARD_ID;
        f.FilterIndex = i / 2;
        f.FilterType = FDCAN_FILTER_DUAL;
        f.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
        f.FilterID1 = ids[i];
        f.FilterID2 = (i + 1 < n) ? ids[i + 1] : ids[i];
        (void)HAL_FDCAN_ConfigFilter(&EV27_MAIN_FDCAN, &f);
    }

    /* GUI 29-bit filter, reject everything else, RX interrupt on, start. */
    fdcan_hardware_init();
}

void ev27_can_queue_rx_from_isr(uint32_t id, const uint8_t *data, uint8_t len, uint32_t time_us)
{
    ev27_rx_frame_t fr;

    if (ev27_rx_queue == NULL) {
        return;
    }
    if (len > sizeof(fr.data)) {
        len = sizeof(fr.data);
    }
    fr.id = id;
    fr.time_us = time_us;
    fr.len = len;
    memcpy(fr.data, data, len);
    (void)osMessageQueuePut(ev27_rx_queue, &fr, 0, 0);   /* full queue: frame dropped */
}

void ev27_can_service(void)
{
    /* After bus-off the FDCAN sets CCCR.INIT and stays off the bus until
     * software clears it. Clearing it starts the normal recovery (128 x 11
     * recessive bits) and then the node is back. */
    if ((EV27_MAIN_FDCAN.Instance->PSR & FDCAN_PSR_BO) != 0u) {
        CLEAR_BIT(EV27_MAIN_FDCAN.Instance->CCCR, FDCAN_CCCR_INIT);
    }
}

/* ---- ev27_port_* for ev27_node ------------------------------------------- */

int ev27_port_send(uint32_t id, const uint8_t *data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef h;
    uint8_t buf[64];

    if (len > 64u) {
        return -1;
    }
    memset(&h, 0, sizeof(h));
    h.Identifier = id;
    h.IdType = FDCAN_STANDARD_ID;
    h.TxFrameType = FDCAN_DATA_FRAME;
    h.DataLength = len_to_dlc(len);
    h.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    h.BitRateSwitch = (EV27_MAIN_DATA_BITRATE != EV27_MAIN_NOMINAL_BITRATE) ? FDCAN_BRS_ON : FDCAN_BRS_OFF;
    h.FDFormat = FDCAN_FD_CAN;
    h.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    /* Pad to the DLC size with zeros. */
    memset(buf, 0, sizeof(buf));
    memcpy(buf, data, len);

    return (fdcan2_tx(&h, buf) == HAL_OK) ? 0 : -1;
}

uint32_t ev27_port_time_us(void)
{
    return __HAL_TIM_GET_COUNTER(&EV27_US_TIMER);
}

uint32_t ev27_port_uptime_ms(void)
{
    return osKernelGetTickCount();   /* configTICK_RATE_HZ = 1000 */
}

void ev27_port_can_errors(uint8_t *tec, uint8_t *rec)
{
    FDCAN_ErrorCountersTypeDef c;
    if (HAL_FDCAN_GetErrorCounters(&EV27_MAIN_FDCAN, &c) == HAL_OK) {
        *tec = (uint8_t)(c.TxErrorCnt > 255u ? 255u : c.TxErrorCnt);
        *rec = (uint8_t)(c.RxErrorCnt > 255u ? 255u : c.RxErrorCnt);
    }
}

void ev27_port_reboot_to_bootloader(void)
{
    /* Deliberately does nothing on the BMS. There is no CAN bootloader yet, and
     * a reset would also clear a latched AMS fault and close the shutdown
     * circuit, which only a person may do (NRST or a power cycle). */
}

void ev27_port_identify(void)
{
    s_identify_until_ms = ev27_port_uptime_ms() + 5000u;   /* nothing blinks yet: no spare LED */
}

bool ev27_identify_active(void)
{
    return (int32_t)(s_identify_until_ms - ev27_port_uptime_ms()) > 0;
}
