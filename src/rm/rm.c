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

void startVirtualMachine(void) {
    printf("[RM] Starting Virtual Machine...\n");

    VirtualMachine* vm = createVM(&hardDisk, &physicalMemory);
    if (!vm) {
        printf("[RM] VM initialization failed.\n");
        return;
    }

    runVM(vm);
    destroyVM(vm);

    printf("[RM] VM finished execution.\n");
}