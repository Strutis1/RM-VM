#include "rm.h"

Memory physicalMemory;
CPU realCPU;
HardDisk hardDisk;


void initRealMachine(void) {
    printf("[RM] Initializing Real Machine...\n");

    RMinitMemory(&physicalMemory);
    initCPU(&realCPU);
    initDisk(&hardDisk, 0);
    IOinit();

    printf("[RM] Initialization complete.\n");
}