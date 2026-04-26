#include "cab_drivers.h"

CABMessage ProcessCABMessage(uint8_t RxData[8]) {
    CABMessage message;
    message.current = RxData[4]<<24 + RxData[3]<<16 + RxData[2]<<8 + RxData[1];
    if(RxData[5] & 1)
        message.error = RxData[5] & 0xFE;
    else
        message.error = OK;
    message.productName[0] = RxData[6];
    message.productName[1] = RxData[5];

    message.softwareRevision = RxData[7];

    return message;
}

