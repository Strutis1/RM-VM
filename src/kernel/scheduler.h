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
#include <stdlib.h>
#include "../../include/common.h"
#include "../utils/utils.h"
#include "../vm/vm.h" 
#include "../vm/vm_cpu.h"

#define SCHEDULER_MAX_PRIORITY 100
#define SCHEDULER_MIN_PRIORITY 0

// forward declarations for pointers
struct Process;
struct Scheduler;
struct Kernel;
typedef int (*ProcessEntry)(struct Process*, struct Scheduler*, struct Kernel*, uint16_t);


// // defining process states
// #define PROC_RUNNING 0
// #define PROC_BLOCK 1
// #define PROC_READY 2
// #define PROC_SUSP_BLOCK 3
// #define PROC_SUSP_READY 4


typedef enum {
    START_STOP,
    IDLE,
    JOB_GOVERNOR,
    LOADER,
    IO_HANDLER,
    INTERRUPT_HANDLER
} ProcType;

typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    SUSP_READY,
    SUSP_BLOCKED
} ProcState;

typedef struct {
    uint16_t IC;
    uint16_t R[REG_COUNT];
    uint16_t SF;
    uint16_t MODE;
    uint16_t PTR;
    uint16_t PI;
    uint16_t SI;
    uint16_t TI;
} CPUContext;

typedef struct Process {
    unsigned char pid;
    bool sysProc; // would usually say that should use char but idfk
    ProcType procType;
    const char* pname;

    ProcState state; //small size, ergonomic and we only need a few states anyway
    CPUContext context;

    VirtualMachine* vm; // VM instance backing this process
    ProcessEntry entry;
    uint16_t fileId;
    unsigned char parentPid; //for Loader->JobGovernor signaling
    uint8_t loaded; //0 not requested, 2 loading, 1 loaded, 3 done
    uint8_t waitingResId; // resource this process is waiting on (0 = none)
    uint8_t swapped; // whether VM memory is swapped out
    uint16_t swapSlot; // swap slot id (pid-based by default)

} Process;

Process* initProcess(unsigned char pid,
                     bool sysProc,
                     const char* processName,
                     ProcType type,
                     VirtualMachine* vm,
                     ProcessEntry entry,
                     uint16_t fileId);

typedef struct {
    Process* schedule[SCHEDULER_MAX_PRIORITY];
    Process* current; // Need for context switcher
    Process* selfProc;
    unsigned short cycle;
    int prio_min, prio_max;
    unsigned char pid;
} Scheduler;

// toggle chatty scheduler logs
extern bool schedulerVerbose;

Process** initSchedule(Process* schedule[]);
bool removeProcP(Scheduler* sch, unsigned char _pid);
bool removeProcN(Scheduler* sch, const char* procName);
Scheduler* initScheduler();
void blockCurrent(Scheduler* sch);
void unblockProcess(Scheduler* sch, unsigned char pid);
bool addProc(Scheduler* sch, Process* proc);
void runCycle(Scheduler* sch, struct Kernel* kernel);
void schedulerSetVerbose(bool enabled);
