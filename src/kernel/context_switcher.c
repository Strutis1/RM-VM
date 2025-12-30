#include "context_switcher.h"
#include "../utils/utils.h"
#include "../../include/registers.h"
#include <string.h>

void saveContext(Process* proc) {
    if (!proc) return;

    proc->context.IC   = realCPU.IC;
    memcpy(proc->context.R, realCPU.R, sizeof(realCPU.R));
    proc->context.SF   = realCPU.SF;
    proc->context.MODE = realCPU.MODE;
    proc->context.PTR  = realCPU.PTR;
    proc->context.PI   = realCPU.PI;
    proc->context.SI   = realCPU.SI;
    proc->context.TI   = realCPU.TI;

    proc->state = READY;
    if (schedulerVerbose) _log("[SCHEDULER] Saved context\n");
}

void loadContext(Process* proc) {
    if (!proc) return;

    realCPU.IC   = proc->context.IC;
    memcpy(realCPU.R, proc->context.R, sizeof(realCPU.R));
    realCPU.SF   = proc->context.SF;
    realCPU.MODE = proc->context.MODE;
    realCPU.PTR  = proc->context.PTR;
    realCPU.PI   = proc->context.PI;
    realCPU.SI   = proc->context.SI;
    realCPU.TI   = proc->context.TI;

    proc->state = RUNNING;
    if (schedulerVerbose) _log("[SCHEDULER] Loaded context\n");
}

void contextSwitch(Scheduler* sch) {
    if (!sch) return;

    if (sch->current && sch->current->state == RUNNING) {
        saveContext(sch->current);
    }

    Process* next = NULL;
    for (int i = sch->prio_max; i >= sch->prio_min; --i) {
        Process* cand = sch->schedule[i];
        if (!cand) continue;
        if (cand->state != READY) continue;
        if (!cand->vm) continue;
        next = cand;
        break;
    }

    if (!next) return;

    loadContext(next);
    sch->current = next;
}
