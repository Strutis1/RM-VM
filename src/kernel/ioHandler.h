#ifndef IOHANDLER_H
#define IOHANDLER_H


#include <stdint.h>
#include "scheduler.h"
#include "resourceManager.h"
#include "kernel.h"

int ioHandlerEntry(Process* proc, Scheduler* scheduler, Kernel* kernel, uint16_t fileId);
bool enqueueIORequest(Process* proc, uint16_t code, uint16_t r0, uint16_t r1, uint16_t r2);


#endif
