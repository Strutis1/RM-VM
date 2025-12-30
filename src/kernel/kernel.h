#pragma once
#include "scheduler.h"
#include "../../include/memory.h"
#include "../../include/registers.h"
#include "../../include/disk.h"
#include "../../include/channel_device.h"
#include "../../include/timer.h"

typedef struct Kernel {
    // software layer
    Scheduler* scheduler;
    Process* current;

    // hardware layer
    Memory* memory;
    CPU* cpu;
    HardDisk* disk;
    ChannelDevice* channel;
    Timer* timer;

    // runtime bookkeeping
    unsigned long tick;
    uint16_t lastPI;
    uint16_t lastSI;
    uint16_t lastTI;
    Process* lastVM;      // last VM-owning process that ran
    uint16_t lastTrapPid; // pid of the process that trapped/issued SI/PI
} Kernel;


void initKernelState(Kernel* k, Scheduler* sch, Memory* mem, CPU* cpu,
                     HardDisk* disk, ChannelDevice* ch, Timer* t);
int kernelHandleInterrupts(Kernel* k);
int kernelStep(Kernel* kernel);
