#include "kernel.h"
#include "../utils/utils.h"
#include "../../include/interrupts.h"

int kernelHandleInterrupts(Kernel* k) {
    if (!k || !k->cpu) return -1;

    CPU* cpu = k->cpu;

    // Sync timer latch into CPU when it fires
    if (k->timer && checkTimerInterrupt(k->timer)) {
        raiseTimerInterrupt(TI_EXPIRED);
    }

    if (cpu->SI == SI_HALT) {
        _log("[KERNEL] HALT interrupt received. Stopping CPU.\n");
        return 0;
    }

    if (cpu->SI == SI_READ) {
        _log("[KERNEL] READ interrupt acknowledged.\n");
        cpu->SI = SI_NONE;
    }

    if (cpu->SI == SI_WRITE) {
        _log("[KERNEL] WRITE interrupt acknowledged.\n");
        cpu->SI = SI_NONE;
    }

    if (cpu->SI == SI_SYS) {
        _log("[KERNEL] SYS interrupt acknowledged.\n");
        cpu->SI = SI_NONE;
    }

    if (cpu->PI != PI_NONE) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[KERNEL] Program interrupt (code %u)\n", cpu->PI);
        _log(buf);
        cpu->PI = PI_NONE;
    }

    if (cpu->TI != TI_NONE) {
        _log("[KERNEL] Timer interrupt acknowledged.\n");
        cpu->TI = TI_NONE;
    }

    return 1;
}
