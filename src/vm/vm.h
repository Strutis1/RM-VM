#pragma once


#include "../../include/common.h"
#include "../../include/disk.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/io.h"
#include "../../include/memory.h"
#include "../../include/registers.h"


typedef struct {
    HardDisk* disk;
    Instruction* instructions;
    Device* device;
    Channel* channel;
    Memory* memory;
    VM_CPU* vm_cpu;
} VirtualMachine;



VirtualMachine createVM();