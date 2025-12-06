#include "scheduler.h"
#include <stdlib.h>

Process** initSchedule(Process* sch[]) {
    if (!sch) return NULL;
    for (int i = 0; i < SCHEDULER_MAX_PRIORITY; ++i) {
        sch[i] = NULL;
    }
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
    return sched;
}
