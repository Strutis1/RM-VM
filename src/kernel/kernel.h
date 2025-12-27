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
} Kernel;

int kernelHandleInterrupts(Kernel* k);
