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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../../include/common.h"

#define SCHEDULER_MAX_PRIORITY 100
#define SCHEDULER_MIN_PRIORITY 0



typedef struct {
    unsigned char pid;
    bool sysProc;
    const char* pname;
} Process;



typedef struct {
    Process* selfProc;
    Process* schedule[SCHEDULER_MAX_PRIORITY];
    unsigned short cycle;
} Scheduler;


Scheduler* initScheduler();