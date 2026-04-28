#include "cab_drivers.h"

cab_message_t process_cab_message(uint8_t *RxData)
{
    cab_message_t message;
    message.current = (RxData[4] << 24) | (RxData[3] << 16) | (RxData[2] << 8) | RxData[1];
    bool error_present = RxData[5] & 1;

    if (error_present)
        message.error_status = RxData[5] & ERROR_INFORMATION_BITMASK;
    else
        message.error_status = OK;
    message.productName[0] = RxData[6];
    message.productName[1] = RxData[5];

    message.softwareRevision = RxData[7];

    return message;
}
