#ifndef INTERRUPTHANDLER_H
#define INTERRUPTHANDLER_H

#include <stdint.h>
#include "scheduler.h"
#include "kernel.h"

//Interrupt handler process: checks PI/SI/TI each tick
int interruptHandlerEntry(Process* proc, Scheduler* scheduler, Kernel* kernel, uint16_t fileId);

#endif