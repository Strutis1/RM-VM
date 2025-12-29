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
    // Reserved for more advanced switching; current templateCycle handles selection
    (void)sch;
}
