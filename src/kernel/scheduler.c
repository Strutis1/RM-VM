#include "scheduler.h"
#include "context_switcher.h"
#include "../../include/disk.h"
#include "resourceManager.h"
#include "swapper.h"
#include "ioHandler.h"
#include "kernel.h"
#include <stdlib.h>
#include <string.h>

// Global disk instance provided by RM init
extern HardDisk hardDisk;

bool schedulerVerbose = false;

Process* initProcess(unsigned char pid,
                     bool sysProc,
                     const char* processName,
                     ProcType type,
                     VirtualMachine* vm,
                     ProcessEntry entry,
                     uint16_t fileId){
    Process* tmp = (Process*)calloc(1, sizeof(Process));
    if (!tmp) return NULL;

    tmp->pid = pid;
    tmp->sysProc = sysProc;
    tmp->pname = processName;
    tmp->procType = type;
    tmp->state = READY;
    tmp->vm = vm;
    tmp->entry = entry;
    tmp->fileId = fileId;
    tmp->parentPid = 0;
    tmp->loaded = 0;
    tmp->swapped = 0;
    tmp->swapSlot = pid;

    return tmp;
}


Process** initSchedule(Process* sch[]) {
    if (!sch) return NULL;

    for (int i = 0; i < SCHEDULER_MAX_PRIORITY; ++i)
        sch[i] = NULL;

    return sch;
}

Scheduler* initScheduler() {
    Process* selfProc = (Process*)calloc(1, sizeof(Process));
    if (!selfProc) return NULL;

    selfProc->pid = 97;
    selfProc->sysProc = true;
    selfProc->pname = "scheduler";
    selfProc->state = READY;
    selfProc->vm = NULL;

    Scheduler* sched = (Scheduler*)calloc(1, sizeof(Scheduler));
    if (!sched) {
        free(selfProc);
        return NULL;
    }

    initSchedule(sched->schedule);

    sched->selfProc = selfProc;
    sched->schedule[97] = selfProc;

    sched->cycle = 0;
    sched->pid = 97;
    sched->prio_max = 97;
    sched->prio_min = 97;

    return sched;
}


bool addProc(Scheduler* sch, Process* proc) {
    if (!sch || !proc) return false;

    sch->pid++;
    proc->pid = sch->pid;

    if (proc->sysProc) {
        for (int i = sch->prio_max; i >= 80; --i) {
            if (sch->schedule[i] == NULL) {
                sch->schedule[i] = proc;

                if (i > sch->prio_max) sch->prio_max = i;
                if (i < sch->prio_min) sch->prio_min = i;

                return true;
            }
        }
    }
    else {
        for (int i = 79; i > 0; --i) {
            if (sch->schedule[i] == NULL) {
                sch->schedule[i] = proc;

                if (i < sch->prio_min) sch->prio_min = i;

                return true;
            }
        }
    }
    return false;
}

bool removeProcP(Scheduler* sch, unsigned char _pid) {
    if (!sch || _pid == 0) return false;

    for (int i = SCHEDULER_MIN_PRIORITY; i < SCHEDULER_MAX_PRIORITY; ++i) {
        if (sch->schedule[i] && sch->schedule[i]->pid == _pid) {
            Process* doomed = sch->schedule[i];
            sch->schedule[i] = NULL;
            if (doomed) {
                if (doomed->vm) {
                    destroyVM(doomed->vm);
                    doomed->vm = NULL;
                }
                free(doomed);
            }
            return true;
        }
    }
    return false;
}

bool removeProcN(Scheduler* sch, const char* procName) {
    if (!sch || !procName) return false;

    for (int i = SCHEDULER_MIN_PRIORITY; i < SCHEDULER_MAX_PRIORITY; ++i) {
        if (sch->schedule[i] && strcmp(sch->schedule[i]->pname, procName) == 0) {
            // free(sch->schedule[i]); // this probably stupid
            sch->schedule[i] = NULL;
            return true;
        }
    }

    return false;
}

void blockCurrent(Scheduler* sch) {
    if (!sch || !sch->current) return;
    sch->current->state = BLOCKED;
    sch->current = NULL;
}

void unblockProcess(Scheduler* sch, unsigned char pid) {
    if (!sch) return;
    for (int i = SCHEDULER_MIN_PRIORITY; i < SCHEDULER_MAX_PRIORITY; ++i) {
        if (sch->schedule[i] && sch->schedule[i]->pid == pid) {
            sch->schedule[i]->state = READY;
            return;
        }
    }
}


void templateCycle(Scheduler* sch, int dispersion) {
    if (!sch) return;

    int window = dispersion < 0 ? 0 : dispersion;
    int low = sch->prio_max - window;
    if (low < sch->prio_min) low = sch->prio_min;
    if (low < SCHEDULER_MIN_PRIORITY) low = SCHEDULER_MIN_PRIORITY;

    for (int i = sch->prio_max; i >= low; --i) {
        Process* next = sch->schedule[i];
        if (!next || next->state != READY) continue;

        if (sch->current && sch->current != next) saveContext(sch->current);

        sch->current = next;

        if (next->entry) {
            int rc = next->entry(next, (struct Scheduler*)sch, (struct Kernel*)NULL, next->fileId);
            if (rc == 0) {
                removeProcP(sch, next->pid);
                sch->current = NULL;
                continue;
            } else if (rc < 0) {
                requestResource(&resourceManager, 1, next);
                sch->current = NULL;
                continue;
            } else {
                next->state = READY;
                sch->current = NULL;
                continue;
            }
        }

        if (next->vm) {
            loadContext(next);
            int vm_status = runOperations(next->vm);

            if (vm_status == 1) {
                removeProcP(sch, next->pid);
                sch->current = NULL;
            } else if (vm_status == 2) {
                if (!requestResource(&resourceManager, 1, next)) {
                    next->state = BLOCKED;
                }
                sch->current = NULL;
            } else if (vm_status < 0) {
                removeProcP(sch, next->pid);
                sch->current = NULL;
            } else {
                next->state = READY;
            }
        }
    }
}


void sysCycle(Scheduler* sch) {
    if (schedulerVerbose) _log("[SCHEDULER] running system cycle\n");
    templateCycle(sch, 20);
}

void dispersedSysCycle(Scheduler* sch) {
    if (schedulerVerbose) _log("[SCHEDULER] running dispersed system cycle\n");
    templateCycle(sch, 2);
}

void fullCycle(Scheduler* sch) {
    if (schedulerVerbose) _log("[SCHEDULER] running full cycle\n");
    templateCycle(sch, sch->prio_max - sch->prio_min);
}

// Run a fixed priority range [hi..lo] inclusive
static void runRange(Scheduler* sch, struct Kernel* kernel, int hi, int lo) {
    if (!sch) return;
    if (hi >= SCHEDULER_MAX_PRIORITY) hi = SCHEDULER_MAX_PRIORITY - 1;
    if (lo < SCHEDULER_MIN_PRIORITY) lo = SCHEDULER_MIN_PRIORITY;

    for (int i = hi; i >= lo; --i) {
        Process* next = sch->schedule[i];
        if (!next || next->state != READY) continue;

        if (sch->current && sch->current != next) saveContext(sch->current);

        sch->current = next;
        if (kernel) kernel->current = next;
        if (kernel && next->vm) {
            kernel->lastVM = next;
            kernel->lastTrapPid = next->pid;
        }

        // CPU resource arbitration
        if (!requestResource(&resourceManager, 3, next)) {
            next->state = BLOCKED;
            sch->current = NULL;
            if (kernel && kernel->current == next) kernel->current = NULL;
            continue;
        }

        if (next->entry) {
            int rc = next->entry(next, (struct Scheduler*)sch, kernel, next->fileId);
            if (rc == 0) {
                removeProcP(sch, next->pid);
                sch->current = NULL;
                releaseResource(&resourceManager, 3, sch);
                if (kernel && kernel->current == next) kernel->current = NULL;
                continue;
            } else if (rc < 0) {
                next->state = BLOCKED;
                sch->current = NULL;
                releaseResource(&resourceManager, 3, sch);
                if (kernel && kernel->current == next) kernel->current = NULL;
                continue;
            } else {
                next->state = READY;
                sch->current = NULL;
                releaseResource(&resourceManager, 3, sch);
                if (kernel && kernel->current == next) kernel->current = NULL;
                continue;
            }
        }

        if (next->vm) {
            loadContext(next);
            // Memory resource for VM execution
            if (!requestResource(&resourceManager, 2, next)) {
                next->state = BLOCKED;
                sch->current = NULL;
                releaseResource(&resourceManager, 3, sch);
                if (kernel && kernel->current == next) kernel->current = NULL;
                continue;
            }
            // If swapped out, bring back in
            if (next->swapped) {
                if (!swapInVM(&hardDisk, next->vm, next->swapSlot)) {
                    _log("[SWAP] Failed to swap in VM\n");
                    next->state = SUSP_BLOCKED;
                    releaseResource(&resourceManager, 2, sch);
                    releaseResource(&resourceManager, 3, sch);
                    if (kernel && kernel->current == next) kernel->current = NULL;
                    continue;
                }
                next->swapped = 0;
                _log("[SWAP] VM swapped in\n");
            }
            int vm_status = runOperations(next->vm);
            if (kernel) kernel->lastTrapPid = next->pid;

            if (vm_status == 1) {
                removeProcP(sch, next->pid);
                sch->current = NULL;
            } else if (vm_status == 2) {
                bool queued = enqueueIORequest(next,
                                               next->vm->trap_code,
                                               next->vm->trap_r0,
                                               next->vm->trap_r1,
                                               next->vm->trap_r2);
                if (queued) {
                    // swap out while waiting on I/O
                    if (!next->swapped) {
                        if (swapOutVM(&hardDisk, next->vm, next->swapSlot)) {
                            next->swapped = 1;
                            _log("[SWAP] VM swapped out during I/O wait\n");
                        }
                    }
                } else {
                    _log("[SCHEDULER] I/O enqueue failed; will retry when process is rescheduled\n");
                }
                next->state = SUSP_BLOCKED;
                sch->current = NULL;
            } else if (vm_status < 0) {
                removeProcP(sch, next->pid);
                sch->current = NULL;
            } else {
                next->state = READY;
            }
            releaseResource(&resourceManager, 2, sch);
        }

        releaseResource(&resourceManager, 3, sch);
        if (kernel && kernel->current == next) kernel->current = NULL;
    }
}

void runCycle(Scheduler* sch, struct Kernel* kernel) {
    if (!sch) return;
    // System pass: top 20 priorities (99..80)
    runRange(sch, kernel, 99, 80);
    // User pass: remaining priorities (79..0)
    runRange(sch, kernel, 79, 0);
    sch->cycle++;
    if (sch->cycle > 2) sch->cycle = 0;
}

void schedulerSetVerbose(bool enabled) {
    schedulerVerbose = enabled;
}
