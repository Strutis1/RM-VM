#include "interruptHandler.h"
#include "../utils/utils.h"
#include "resourceManager.h"

// Wake the first process of a given type
void wakeProcessByType(Scheduler* scheduler, ProcType type) {
    if (!scheduler) return;
    for (int i = SCHEDULER_MIN_PRIORITY; i < SCHEDULER_MAX_PRIORITY; ++i) {
        Process* p = scheduler->schedule[i];
        if (p && p->procType == type && p->state == BLOCKED) {
            p->state = READY;
            return;
        }
    }
}

// Find process by pid
static Process* findProcessByPid(Scheduler* scheduler, unsigned char pid) {
    if (!scheduler) return NULL;
    for (int i = SCHEDULER_MIN_PRIORITY; i < SCHEDULER_MAX_PRIORITY; ++i) {
        Process* p = scheduler->schedule[i];
        if (p && p->pid == pid) return p;
    }
    return NULL;
}

int interruptHandlerEntry(Process* proc, Scheduler* scheduler, Kernel* kernel, uint16_t fileId) {
    (void)proc;
    (void)fileId;
    if (!kernel) return 1;

    // Grab timer resource (id=4) while we poke at interrupts
    bool tmrHeld = requestResource(&resourceManager, 4, scheduler ? scheduler->current : NULL);

    kernelHandleInterrupts(kernel);

    // Snapshot current interrupt causes (from kernel bookkeeping, set during handle)
    uint16_t pi = kernel->lastPI;
    uint16_t si = kernel->lastSI;
    uint16_t ti = kernel->lastTI;

    // Try to tie interrupts to the last VM that ran
    Process* vmOwner = kernel->lastVM;
    if (!vmOwner && kernel->lastTrapPid) {
        vmOwner = findProcessByPid(scheduler, (unsigned char)kernel->lastTrapPid);
    }

    // Quick reactions:
    if (si == SI_READ || si == SI_WRITE) {
        // nudge I/O handler to pick it up
        wakeProcessByType(scheduler, IO_HANDLER);
    }
    if (si == SI_HALT && vmOwner) {
        // VM halted: clean it up and mark done
        if (vmOwner->vm) {
            destroyVM(vmOwner->vm);
            vmOwner->vm = NULL;
        }
        vmOwner->loaded = 3;
        vmOwner->state = READY;
        // wake parent JG if there is one
        if (vmOwner->parentPid) {
            Process* parent = findProcessByPid(scheduler, vmOwner->parentPid);
            if (parent) parent->state = READY;
        }
    }
    if (pi != PI_NONE) {
        // tell job governors a VM blew up
        wakeProcessByType(scheduler, JOB_GOVERNOR);
        if (vmOwner && vmOwner->vm) {
            destroyVM(vmOwner->vm);
            vmOwner->vm = NULL;
            vmOwner->loaded = 3;
            vmOwner->state = READY;
            if (vmOwner->parentPid) {
                Process* parent = findProcessByPid(scheduler, vmOwner->parentPid);
                if (parent) parent->state = READY;
            }
        }
    }
    if (ti == TI_EXPIRED && scheduler) {
        // preemption already requested; nothing else for now
    }

    // Clear latched values after handling
    kernel->lastPI = 0;
    kernel->lastSI = 0;
    kernel->lastTI = 0;

    if (tmrHeld) {
        releaseResource(&resourceManager, 4, scheduler);
    }
    return 1; // stay ready
}
