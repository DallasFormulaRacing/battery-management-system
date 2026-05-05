#include "cab_drivers.h"



cab_message_t parse_cab_message(const uint8_t *RxData)
{
    cab_message_t message;
    message.current = ((RxData[4] << 24) | (RxData[3] << 16) | (RxData[2] << 8) | RxData[1]) - CAB_0_CURRENT_OFFSET;
    bool error_present = RxData[5] & ERORR_OCCURED_BITMASK;

    if (error_present)
        message.error_status = RxData[5] & ERROR_INFORMATION_BITMASK;
    else
        message.error_status = OK;
    message.productName[0] = RxData[6];
    message.productName[1] = RxData[5];

    message.softwareRevision = RxData[7];

    return message;
}

void process_cab_message(const uint8_t* data) {
    cab_message_t message_data = parse_cab_message(data);
    if(message_data.error_status != OK) {
        //TODO deal with errors here
    } else {

    hbms.pack->instantaneous_current = message_data.current;
    }
}
