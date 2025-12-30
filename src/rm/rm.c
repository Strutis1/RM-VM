#include "rm.h"
#include "../utils/utils.h"

// ───────────────────────────────
// Globalūs Real Machine objektai
// ───────────────────────────────
Memory physicalMemory;
CPU realCPU;
HardDisk hardDisk;
Timer systemTimer;

// ───────────────────────────────
// Kernel initialization
// ───────────────────────────────
bool initKernel(Memory* mem, CPU* cpu, HardDisk* hard) {
    if (!mem || !cpu || !hard) return false;

    RMinitMemory(mem);
    initCPU(cpu);
    clearAllInterrupts();

    initDisk(hard, 0);
    initTimer(&systemTimer, TICK_LIMIT);
    IOinit();

    _log("[RM] Kernel initialization complete.\n");
    return true;
}

// ───────────────────────────────
// Real Machine init
// ───────────────────────────────
void initRealMachine(void) {
    _log("[RM] Initializing Real Machine...\n");

    if (initKernel(&physicalMemory, &realCPU, &hardDisk))
        _log("[RM] Initialization complete.\n");
    else
        _log("[RM] Initialization failed.\n");
}

// ───────────────────────────────
// VM startup
// ───────────────────────────────
void startVirtualMachine(void) {
    _log("[RM] Starting Virtual Machine...\n");

    VirtualMachine* vm = createVM(&hardDisk, &physicalMemory, 1);
    if (!vm) {
        _log("[RM] VM initialization failed.\n");
        return;
    }

    runVM(vm);
    destroyVM(vm);

    _log("[RM] VM finished execution.\n");
}
