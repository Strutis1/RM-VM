#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "device.h"



typedef struct Channel {
    uint8_t id;
    uint8_t busy;
    uint8_t ready;
    uint8_t interruptFlag;
    Device *device;
} Channel;


Channel *createChannel(uint8_t id, struct Device *dev);
bool channelRead(Channel *ch, uint16_t addr, uint8_t *buffer);
bool channelWrite(Channel *ch, uint16_t addr, const uint8_t *buffer);