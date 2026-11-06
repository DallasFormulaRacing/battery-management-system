#include "cab_drivers.h"

cab_message_t parse_cab_message(const uint8_t *rx_data)
{
    cab_message_t message;
    message.current = ((rx_data[4] << 24) | (rx_data[3] << 16) | (rx_data[2] << 8) | rx_data[1]) - CAB_0_CURRENT_OFFSET;
    bool error_present = rx_data[5] & ERROR_OCCURRED_BITMASK;

    if (error_present)
        message.error_status = rx_data[5] & ERROR_INFORMATION_BITMASK;
    else
        message.error_status = OK;

    message.productName[0] = rx_data[6];
    message.productName[1] = rx_data[5];

    message.softwareRevision = rx_data[7];

    return message;
}

void process_cab_message(const uint8_t *rx_data)
{
    cab_message_t message_data = parse_cab_message(rx_data);
    if (message_data.error_status != OK)
    {
        // TODO deal with errors here
    }
    else
    {
        hbms.pack->instantaneous_current = message_data.current;
    }
}

void set_cab_output_frequency(const uint16_t period_ms)
{
    uint8_t *frequency_message[6];
    frequency_message[0] = FRAME_PERIOD_DATA_LENGTH;
    frequency_message[1] = WRITE_BY_DATA_IDENTIFIER;

    frequency_message[2] = (uint8_t)(FRAME_PERIOD_SUB_FUNCTION & 0xFF);
    frequency_message[3] = (uint8_t)(FRAME_PERIOD_DATA_LENGTH >> 8);

    frequency_message[4] = (uint8_t)(period_ms & 0xFF);
    frequency_message[5] = (uint8_t)(period_ms >> 8);

    uint32_t txHeader = can_id_build(CAN_PRIORITY_P1, CAB_ID, FRAME_PERIOD_SUB_FUNCTION, NODE_BMS);
    fdcan_send(txHeader, frequency_message, FDCAN_DLC_BYTES_6);
}
