#pragma once


#include <stdint.h>

#define DISK_SECTOR_SIZE 16
#define DISK_SECTOR_COUNT 64

typedef struct {
    uint8_t id;
    uint8_t busy;
    uint8_t ready;
    uint8_t interruptFlag;
    uint16_t head;
    uint8_t storage[DISK_SECTOR_COUNT][DISK_SECTOR_SIZE];
} HardDisk;