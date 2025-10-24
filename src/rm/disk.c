#include "rm.h"
#include "../../include/disk.h"
#include "../../include/common.h"
#include <string.h>
#include <stdio.h>

void initDisk(HardDisk *disk, uint8_t id) {
    disk->id = id;
    disk->busy = 0;
    disk->ready = 1;
    disk->interruptFlag = 0;
    disk->head = 0;

    memset(disk->storage, 0, sizeof(disk->storage));
}

bool readDisk(HardDisk *disk, uint16_t sector, uint8_t *buffer) {
    if (sector >= DISK_SECTOR_COUNT) return false;
    if (disk->busy) return false;

    disk->busy = 1;
    memcpy(buffer, disk->storage[sector], DISK_SECTOR_SIZE);
    disk->busy = 0;
    disk->interruptFlag = 1;

    return true;
}

bool writeDisk(HardDisk *disk, uint16_t sector, const uint8_t *buffer) {
    if (sector >= DISK_SECTOR_COUNT) return false;
    if (disk->busy) return false;

    disk->busy = 1;
    memcpy(disk->storage[sector], buffer, DISK_SECTOR_SIZE);
    disk->busy = 0;
    disk->interruptFlag = 1;

    return true;
}

void simulateDiskInterrupt(HardDisk *disk) {
    if (disk->interruptFlag) {
        disk->interruptFlag = 0;
    }
}
