#include "../../include/channel_device.h"
#include "../../include/disk.h"
#include "../../include/interrupts.h"
#include "../../include/registers.h"
#include "../../include/memory.h"
#include <stdbool.h>
#include <stdio.h>

extern CPU realCPU;
extern Memory physicalMemory;
extern HardDisk hardDisk;

static ChannelDevice diskChannel;
bool diskInitialized = false;


void raiseSystemInterrupt(uint16_t code) {
    if (code < SI_SYS)
        realCPU.SI = code;
}

void IOinit(void) {
    if (!diskInitialized) {
        initDisk(&hardDisk, 0);
        initChannelDevice(&diskChannel);
        diskInitialized = true;
        printf("[IO] Disk and channel initialized.\n");
    }
}


bool IOread(uint16_t sector, uint16_t destAddr, uint16_t count) {
    if (!diskInitialized) return false;

    diskChannel.ST = CH_SRC_DISK;
    diskChannel.SB = sector;
    diskChannel.DT = CH_DST_SUPER_MEM;
    diskChannel.DB = destAddr;
    diskChannel.COUNT = count;
    diskChannel.OFFSET = 0;

    bool ok = channelXCHG(&diskChannel, &physicalMemory, &hardDisk, &realCPU);
    if (!ok) {
        printf("[IO] Failed to read sector %u\n", sector);
        return false;
    }

    raiseSystemInterrupt(SI_READ);
    printf("[IO] Sector %u read complete, SI_READ interrupt raised.\n", sector);
    return true;
}


// Write supervisor memory data to disk
bool IOwrite(uint16_t sector, uint16_t srcAddr, uint16_t count) {
    if (!diskInitialized) return false;

    diskChannel.ST = CH_SRC_SUPER_MEM;
    diskChannel.SB = srcAddr;
    diskChannel.DT = CH_DST_DISK;
    diskChannel.DB = sector;
    diskChannel.COUNT = count;
    diskChannel.OFFSET = 0;

    bool ok = channelXCHG(&diskChannel, &physicalMemory, &hardDisk, &realCPU);
    if (!ok) {
        printf("[IO] Failed to write sector %u\n", sector);
        return false;
    }

    raiseSystemInterrupt(SI_WRITE);
    printf("[IO] Sector %u write complete, SI_WRITE interrupt raised.\n", sector);
    return true;
}


void IOcheckInterrupts(void) {
    if (diskChannel.busy) return;
    if (realCPU.SI != SI_NONE) {
        printf("[IO] Handling I/O interrupt SI=%u\n", realCPU.SI);
        realCPU.SI = SI_NONE;
    }
}
