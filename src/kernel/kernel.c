#include "kernel.h"
#include "../utils/utils.h"
#include "../../include/interrupts.h"
#include "context_switcher.h"
#include "resourceManager.h"
#include <stdbool.h>




void initKernelState(Kernel* k, Scheduler* sch, Memory* mem, CPU* cpu,
                     HardDisk* disk, ChannelDevice* ch, Timer* t) {
    if (!k) return;
    k->scheduler = sch;
    k->current = NULL;
    k->memory = mem;
    k->cpu = cpu;
    k->disk = disk;
    k->channel = ch;
    k->timer = t;
    k->tick = 0;
    k->lastPI = k->lastSI = k->lastTI = 0;
    k->lastVM = NULL;
    k->lastTrapPid = 0;

    // Initialize global resource manager and core resources
    initResourceManager(&resourceManager);
    createResource(&resourceManager, 1, "IO", 1);
    createResource(&resourceManager, 2, "MEM", 2);
    createResource(&resourceManager, 3, "CPU", 1);
    createResource(&resourceManager, 4, "TMR", 1);
}




int kernelHandleInterrupts(Kernel* k) {
    if (!k || !k->cpu) return -1;

    CPU* cpu = k->cpu;
    Scheduler* sch = k->scheduler;
    Process* cur = sch ? sch->current : NULL;
    bool canBlock = cur && cur->procType != INTERRUPT_HANDLER;
    if (cur) k->lastTrapPid = cur->pid;

    // Sync timer latch into CPU when it fires
    if (k->timer && checkTimerInterrupt(k->timer)) {
        raiseTimerInterrupt(TI_EXPIRED);
    }

    // PI has highest priority
    if (cpu->PI != PI_NONE) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[KERNEL] Program interrupt (code %u)\n", cpu->PI);
        _log(buf);
        k->lastPI = cpu->PI;
        cpu->PI = PI_NONE;
        if (canBlock && sch) blockCurrent(sch);
        return 0; // stop current VM/process
    }

    switch (cpu->SI) {
        case SI_HALT:
            _log("[KERNEL] HALT interrupt received. Stopping CPU.\n");
            k->lastSI = cpu->SI;
            cpu->SI = SI_NONE;
            if (canBlock && sch) blockCurrent(sch);
            return 0;
        case SI_READ:
            _log("[KERNEL] READ interrupt acknowledged (not yet handled).\n");
            k->lastSI = cpu->SI;
            cpu->SI = SI_NONE;
            if (canBlock && sch) blockCurrent(sch);
            return 1;
        case SI_WRITE:
            _log("[KERNEL] WRITE interrupt acknowledged (not yet handled).\n");
            k->lastSI = cpu->SI;
            cpu->SI = SI_NONE;
            if (canBlock && sch) blockCurrent(sch);
            return 1;
        case SI_SYS:
            _log("[KERNEL] SYS interrupt acknowledged (not yet handled).\n");
            k->lastSI = cpu->SI;
            cpu->SI = SI_NONE;
            if (canBlock && sch) blockCurrent(sch);
            return 1;
        case SI_NONE:
            k->lastSI = SI_NONE;
            break;
        default:
            break;
    }

    switch (cpu->TI) {
        case TI_EXPIRED:
            _log("[KERNEL] Timer expired interrupt acknowledged.\n");
            k->lastTI = cpu->TI;
            cpu->TI = TI_NONE;
            if (sch) contextSwitch(sch);
            return 1; // signal preemption
        case TI_NONE:
            k->lastTI = TI_NONE;
            break;
        default:
            _log("[KERNEL] Timer interrupt acknowledged.\n");
            k->lastTI = cpu->TI;
            cpu->TI = TI_NONE;
            return 1;
    }

    return 1;
}

// ticks timer
int kernelStep(Kernel* kernel){
    if (!kernel) return -1;
    if (kernel->timer) tickTimer(kernel->timer);
    return 1;
}
