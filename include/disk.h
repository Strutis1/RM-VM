#pragma once


#include <stdint.h>
#include "common.h"

#define DISK_SECTOR_COUNT 128
#define DISK_SECTOR_SIZE  16

/* should probably make a struct wthat houses diskSectors */

typedef struct {
    uint8_t id;
    uint8_t busy;
    uint8_t ready;
    uint8_t interruptFlag;
    uint16_t head;
    uint8_t storage[DISK_SECTOR_COUNT][DISK_SECTOR_SIZE];
} HardDisk;

void initDisk(HardDisk *disk, uint8_t id);
bool readDisk(HardDisk *disk, uint16_t sector, uint8_t *buffer);
bool writeDisk(HardDisk *disk, uint16_t sector, const uint8_t *buffer);
void simulateDiskInterrupt(HardDisk *disk);