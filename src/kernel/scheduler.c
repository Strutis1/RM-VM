#include "scheduler.h"
#include <stdlib.h>
#include <string.h>

Process* initProcess(unsigned char pid,
                     bool sysProc,
                     const char* processName,
                     VirtualMachine* vm) {
    Process* tmp = (Process*)calloc(1, sizeof(Process));
    if (!tmp) return NULL;

    tmp->pid = pid;
    tmp->sysProc = sysProc;
    tmp->pname = processName;
    tmp->state = PROC_READY;
    tmp->vm = vm;

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
    selfProc->state = PROC_READY;
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
            sch->schedule[i] = NULL;
            return true;
        }
    }
    return false;
}

bool removeProcN(Scheduler* sch, const char* procName) {
    if (!sch || !procName) return false;

    for (int i = SCHEDULER_MIN_PRIORITY; i < SCHEDULER_MAX_PRIORITY; ++i) {
        if (sch->schedule[i] &&
            strcmp(sch->schedule[i]->pname, procName) == 0)
        {
            sch->schedule[i] = NULL;
            return true;
        }
    }

    return false;
}


void templateCycle(Process* schedule[], int prio_min, int prio_max, int dispersion) {
    int _dispersion = dispersion < 0 ? 0 : dispersion;

    if (prio_min - _dispersion < 0) _dispersion = 0;

    for (int i = prio_max; i >= prio_min - _dispersion; --i) {
        if (schedule[i] && schedule[i]->state == PROC_READY && schedule[i]->vm) {
            schedule[i]->state = PROC_RUNNING;
            runVM(schedule[i]->vm);
            schedule[i]->state = PROC_READY;
        }
    }
}


void sysCycle(Scheduler* sch) {
    _log("[SCHEDULER] running system cycle\n");
    templateCycle(sch->schedule, sch->prio_min, sch->prio_max, 0);
}

void dispersedSysCycle(Scheduler* sch) {
    _log("[SCHEDULER] running dispersed system cycle\n");
    templateCycle(sch->schedule,sch->prio_min,sch->prio_max,5);
}

void fullCycle(Scheduler* sch) {
    _log("[SCHEDULER] running full cycle\n");
    templateCycle(sch->schedule, sch->prio_min, sch->prio_max, 80);
}

void runCycle(Scheduler* sch) {
    if (!sch) return;

    switch (sch->cycle) {
        case 0:
            sysCycle(sch);
            break;
        case 1:
            dispersedSysCycle(sch);
            break;
        case 2:
            fullCycle(sch);
            break;
        default:
            sch->cycle = 0;
            return;
    }

    sch->cycle++;
    if (sch->cycle > 2) sch->cycle = 0;
}
