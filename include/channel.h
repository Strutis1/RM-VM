#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


/*
typedef struct {
    uint8_t id;
    uint8_t busy;
    uint8_t ready;
    uint8_t interruptFlag;
    Device *device;
} Channel;
 */

typedef struct {
    char* flag; // read or write
    char* busy; // flag for channel busy y/n
    uint8_t id; // channel ID, not sure why we need it but lets keep it ig (were running 1 vm)
    uint64_t* bin; //the actual data
    //uint8_t interruptFlag; // i dont know what this does so im just keeping it here
} Channel;

char writeChannelData(Channel* chan, uint64_t* binPtr);
uint32_t readChannelData(Channel* dev);