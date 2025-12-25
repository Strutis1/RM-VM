#include "scheduler.h"


// process internals
Process* initProcess(unsigned char pid, bool sysProc, const char* processName, int (*fptr)(void)) {
    Process* tmp = { pid, sysProc, processName, PROC_RUNNING, fptr};
    // will probably need to do some stuff here or chain functions when using to init -> add to schedule :)
    return tmp;
}


// scheduler internals
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

    Scheduler* sched = (Scheduler*)calloc(1, sizeof(Scheduler));
    if (!sched) {
        free(selfProc);
        return NULL;
    }

    initSchedule(sched->schedule);
    sched->selfProc = selfProc;
    sched->schedule[97] = selfProc;
    sched->cycle = 0;
    sched->pid = 0;
    sched->prio_max = 97;
    sched->prio_min = 97;
    return sched;
}

bool inline addProc(Scheduler* sch, Process* proc) {
    if (!sch || !proc) return false;

    sch->pid++;
    proc->pid = sch->pid;
    if (proc->sysProc == true) {
        if (sch->prio_min == sch->prio_max) {
            sch->schedule[sch->prio_max - 1];
            sch->prio_min -= 1;

            return true;
        }

        for (int i = sch->prio_max; i >= 80; --i) {
            if (sch->schedule[i] == NULL) {
                sch->schedule[i] = proc;
                
                sch->prio_max = i > sch->prio_max ? i : sch->prio_max;
                sch->prio_min = i < sch->prio_min ? i : sch->prio_min;
                
                return true;
            }
        }
    }

    for (int i = 79; i > 0; --i) {
        if (sch->schedule[i] == NULL) {
            sch->schedule[i] = proc;

            sch->prio_min = i < sch->prio_min ? i : sch->prio_min;
            
            return true;
        }
    }
}

bool inline removeProcP(Scheduler* sch, unsigned char _pid) {
    if (!sch || _pid == 0) return false;

    for (int i = SCHEDULER_MIN_PRIORITY; i < SCHEDULER_MAX_PRIORITY; ++i) {
        if (sch->schedule[i]->pid == _pid) {
            sch->schedule[i] = NULL;
            return true;
        }
    }
    return false;
}

bool inline removeProcN(Scheduler* sch, const char* procName) {
    if (!sch || !procName) return false;

    for (int i = SCHEDULER_MIN_PRIORITY; i < SCHEDULER_MAX_PRIORITY; ++i) {
        if (strcmp(sch->schedule[i]->pname, procName) == 0) {
            sch->schedule[i] = NULL;
            return true;
        }
    }

    return false;
}


void inline templateCycle(const Process* schedule, int prio_min, int prio_max, int dispersion) {
    int _dispersion = dispersion < 0 ? 0 : dispersion;
    
    // i dont remember what i was cooking here,
    // check dispersions and docs later
    // cooking or smoking is also a great answer i am yet to answer...

    if (prio_min - dispersion <= 0) {
        _log("[Error] bad dispersion: proceeding with 0\n");
        _dispersion = 0;
    }

    for (int i = prio_max; i >= prio_min - _dispersion; --i)
        schedule[i].fptr();
    
}

void inline sysCycle(Scheduler* sch) {
    templateCycle(sch->schedule, sch->prio_min, sch->prio_max, 0);
}

void inline dispersedSysCycle(Scheduler* sch) {
    templateCycle(sch->schedule, sch->prio_min, sch->prio_max, 5);
}

void inline fullCycle(Scheduler* sch) {
    templateCycle(sch->schedule, sch->prio_min, sch->prio_max, 80);
}

void inline runCycle(Scheduler* sch) {
    int dispersion = sch->cycle == 1 ? 5 : 0;
    
    switch(sch->cycle) {
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
            _log("[SCHEDULER] cycle misaligned\n");
    }

    sch->cycle++;
    if (sch->cycle > 2) sch->cycle = 0;
}

