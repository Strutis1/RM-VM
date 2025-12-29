// VM-centric context switching. Each process owns its own VM, so we mostly
// track state transitions rather than copying registers around.

#include "context_switcher.h"
#include "../utils/utils.h"

void saveContext(Process* proc) {
    if (!proc || !proc->vm) return;
    proc->state = PROC_READY;
    _log("[SCHEDULER] Saved VM context\n");
}

void loadContext(Process* proc) {
    if (!proc || !proc->vm) return;
    proc->state = PROC_RUNNING;
    _log("[SCHEDULER] Loaded VM context\n");
}

void contextSwitch(Scheduler* sch) {
    if (!sch) return;

    Process* prev = sch->current;
    Process* next = NULL;

    for (int i = sch->prio_max; i >= sch->prio_min; --i) {
        if (sch->schedule[i] &&
            sch->schedule[i]->state == PROC_READY &&
            sch->schedule[i]->vm) {
            next = sch->schedule[i];
            break;
        }
    }

    if (!next || next == prev) return;

    if (prev) saveContext(prev);

    loadContext(next);
    sch->current = next;
}
