#include "../../include/device.h"
#include "../../include/disk.h"


Device *createDevice(uint8_t id, void *data) {
    Device *dev = (Device*) malloc(sizeof(Device));
    if (!dev) return NULL;

    dev->id = id;
    dev->busy = 0;
    dev->ready = 1;
    dev->interruptFlag = 0;
    dev->data = data;

    return dev;
}

bool deviceRead(Device *dev, uint16_t addr, uint8_t *buffer) {
    if (!dev || !dev->data) return false;
    HardDisk *disk = (HardDisk *)dev->data;
    return readDisk(disk, addr, buffer);
}

bool deviceWrite(Device *dev, uint16_t addr, const uint8_t *buffer) {
    if (!dev || !dev->data) return false;
    HardDisk *disk = (HardDisk *)dev->data;
    return writeDisk(disk, addr, buffer);
}


