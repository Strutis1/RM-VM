/*
make a hash map of buckets 1-100
each process priority is rated from 1 to 100 (Highest prio)

going down by list the first highest has dispersion of 2 up and down
this ensures sys processes dont hog resources and share with lower priority tasks

each thing so it would for example go like this:

use this example:
> 100: scheduler, processor
> 99:  VM
> 98:  wayland protocol
> 97:  cs 1.6

we run 1 clock period of 100, 99, and 98, then repeat until some proc is done
presume in this situation we trancend and no longer require a monitor then bucket 98 is empty
then we run 100, 99, 97 since 98 is empty.

it would probably be wise to run every 3 clock period through all of the scheduled processes in order to avoid hanging and
   crashing processes.

how this could be done is by having an array of pointers, which would act as labels to buckets, thus once the bucket
   is depointed and the dynamic arrays first element is not null we run one cycle through the previously mentioned system of scheduling.
*/


#pragma once

#include "context_switcher.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "../../include/common.h"
#include "../utils/utils.h"

#define SCHEDULER_MAX_PRIORITY 100
#define SCHEDULER_MIN_PRIORITY 0


// defining process states
#define PROC_RUNNING 0
#define PROC_BLOCK 1
#define PROC_READY 2
#define PROC_SUSP_BLOCK 3
#define PROC_SUSP_READY 4


typedef struct {
    unsigned char pid;
    bool sysProc; // would usually say that should use char but idfk
    const char* pname;

    unsigned char state; //small size, ergonomic and we only need a few states anyway
    
    CPUContext context;

    int (*fptr)(void); 
} Process;

Process* initProcess(unsigned char pid, bool sysProc, const char* processName, int (*fptr)(void));

typedef struct {
    Process* schedule[SCHEDULER_MAX_PRIORITY];
    Process* current; // Need for context switcher
    Process* selfProc;
    unsigned short cycle;
    int prio_min, prio_max;
    unsigned char pid;
} Scheduler;

Process** initSchedule(Process* schedule[]);
bool inline removeProcP(Scheduler* sch, unsigned char _pid);
bool inline removeProcN(Scheduler* sch, const char* procName);
Scheduler* initScheduler();