#include "../../include/device.h"
#include "../../include/disk.h"

/*
typedef struct {
    char* flag; // read or write
    char* busy; // flag for channel busy y/n
    uint8_t id; // channel ID, not sure why we need it but lets keep it ig (were running 1 vm)
    uint64_t* bin; //the actual data
    //uint8_t interruptFlag; // i dont know what this does so im just keeping it here
} Channel;
*/

void initDevice(Device* devPtr, uint8_t devID) {
    devPtr->id = devID;
    devPtr->chanPtr = malloc(sizeof *devPtr->chanPtr);
    if(!devPtr->chanPtr) return 1;
    devPtr->chanPtr->flag = NULL;
    devPtr->chanPtr->busy = NULL;
    devPtr->chanPtr->id   = devID;
    devPtr->chanPtr->bin  = NULL;
}

char writeDeviceData(Device* dev, uint64_t* bin) {
    return writeChannelData(dev->chanPtr, bin);
}

uint32_t readDeviceData(Device* dev) {
    return readChannelData(dev->chanPtr);
}