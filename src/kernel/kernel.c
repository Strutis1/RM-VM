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

    switch (cpu->SI) {
        case SI_HALT:
            _log("[KERNEL] HALT interrupt received. Stopping CPU.\n");
            return 0;
        case SI_READ:
            _log("[KERNEL] READ interrupt acknowledged.\n");
            cpu->SI = SI_NONE;
            break;
        case SI_WRITE:
            _log("[KERNEL] WRITE interrupt acknowledged.\n");
            cpu->SI = SI_NONE;
            break;
        case SI_SYS:
            _log("[KERNEL] SYS interrupt acknowledged.\n");
            cpu->SI = SI_NONE;
            break;
        default:
            break;
    }

    switch (cpu->PI) {
        case PI_NONE:
            break;
        default: {
            char buf[64];
            snprintf(buf, sizeof(buf), "[KERNEL] Program interrupt (code %u)\n", cpu->PI);
            _log(buf);
            cpu->PI = PI_NONE;
            break;
        }
    }

    switch (cpu->TI) {
        case TI_NONE:
            break;
        default:
            _log("[KERNEL] Timer interrupt acknowledged.\n");
            cpu->TI = TI_NONE;
            break;
    }

    return 1;
}
