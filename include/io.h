#pragma once



#include <stdint.h>

typedef struct Device {
    uint8_t id;
    uint8_t busy;
    uint8_t ready;
    uint8_t interruptFlag;
    void *data;
    uint8_t (*read)(void *device, uint16_t addr);
    void (*write)(void *device, uint16_t addr, uint8_t value);
} Device;

typedef struct Channel {
    uint8_t id;
    uint8_t busy;
    uint8_t ready;
    uint8_t interruptFlag;
    Device *device;
    uint8_t (*read)(struct Channel *ch, uint16_t addr);
    void (*write)(struct Channel *ch, uint16_t addr, uint8_t value);
} Channel;