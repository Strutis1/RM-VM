#include "../../include/channel.h"
#include "../../include/disk.h"
#include "../../include/interrupts.h"
#include "../../include/registers.h"
#include <stdbool.h>
#include <stdio.h>

static Channel *diskChannel = NULL;


void raiseSystemInterrupt(uint16_t code) {
    if (code >= SI_NONE && code <= SI_HALT)
        realCPU.SI = code;
}



void IOinit(void) {
    if (!diskInitialized) {
        initDisk(&hardDisk, 0);
        diskInitialized = true;
    }

    if (!diskChannel) {
        Device *dev = createDevice(0, &hardDisk);
        diskChannel = createChannel(0, dev);
        printf("[IO] Disk channel initialized.\n");
    }
}


bool IOread(uint16_t sector, uint8_t *buffer) {
    if (!diskChannel) return false;
    bool ok = channelRead(diskChannel, sector, buffer);

    if (!ok) {
        printf("[IO] Failed to read sector %u\n", sector);
        return false;
    }
    raiseSystemInterrupt(SI_READ);
    printf("[IO] Sector %u read complete, SI_READ interrupt raised.\n", sector);

    return true;
}



bool IOwrite(uint16_t sector, const uint8_t *buffer) {
    if (!diskChannel) return false;
    bool ok = channelWrite(diskChannel, sector, buffer);

    if (!ok)
        printf("[IO] Failed to write sector %u\n", sector);
    return ok;
}


void IOcheckInterrupts(void) {
    if (diskChannel && diskChannel->interruptFlag) {
        diskChannel->interruptFlag = 0;
        raiseSystemInterrupt(SI_WRITE);  
        printf("[IO] Disk interrupt triggered.\n");
    }
}
