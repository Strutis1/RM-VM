#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct Device {
    uint8_t id;
    uint8_t busy;
    uint8_t ready;
    uint8_t interruptFlag;
    void *data;
} Device;


Device *createDevice(uint8_t id, void *data);
bool deviceRead(Device *dev, uint16_t addr, uint8_t *buffer);
bool deviceWrite(Device *dev, uint16_t addr, const uint8_t *buffer);