#include "rm.h"
#include "../utils/utils.h"

Memory physicalMemory;
CPU realCPU;
HardDisk hardDisk;
Timer systemTimer;

bool initKernel(Memory* mem, CPU* cpu, HardDisk* hard) {
    if (!mem || !cpu || !hard) return false;

    RMinitMemory(mem);
    initCPU(cpu);
    clearAllInterrupts();

    initDisk(hard, 0);
    initTimer(&systemTimer, TICK_LIMIT);
    IOinit();

    return true;
}

void initRealMachine(void) {
    _log("[RM] Initializing Real Machine...\n");

    if (initKernel(&physicalMemory, &realCPU, &hardDisk))
        _log("[RM] Initialization complete.\n");
    else
        _log("[RM] Initialization failed.\n");
}
