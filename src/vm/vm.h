// #pragma once

// #include "../../include/common.h"
// #include "../../include/instructions.h"
// #include "../../include/interrupts.h"
// #include "../../include/channel_device.h"
// #include "../../include/registers.h"

// #include "vm_cpu.h"
// #include "vm_memory.h"
// #include "vm_program.h"

// typedef struct {
//     Instruction* instructions;
//     Channel* channel;
//     VM_MEMORY* memory;
//     VM_CPU* vm_cpu;
// } VirtualMachine;

// VirtualMachine* createVM(Channel* channel);
#pragma once

#include "../../include/common.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/channel_device.h"
#include "../../include/registers.h"
#include "../../include/disk.h"
#include "../../include/memory.h"

#include "vm_cpu.h"
#include "vm_memory.h"
#include "vm_program.h"

typedef struct VirtualMachine {
    Instruction* instructions;
    ChannelDevice channel;
    VM_MEMORY* memory;
    VM_CPU* vm_cpu;
} VirtualMachine;

VirtualMachine* createVM(HardDisk* disk, Memory* rmMemory);
void destroyVM(VirtualMachine* vm);
void runVM(VirtualMachine* vm);
void loadDemoProgram(void);
