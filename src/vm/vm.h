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
// =======
// #pragma once
// #include "../../include/common.h"
// #include "../../include/instructions.h"
// #include "../../include/interrupts.h"
// #include "../../include/registers.h"
// >>>>>>> 4104e68847041b9d8d6f1b8c3de28571f2adc05c

#include "vm_cpu.h"
#include "vm_memory.h"
#include "vm_channel.h"

typedef struct VirtualMachine {
    ChannelDevice*   channel; //This right now has to be ChannelDevice, could fix later, but dont think it matters rn
    Instruction* instructions; //for testing, but no purpose
    VM_MEMORY* memory;
    VM_CPU*    vm_cpu;
    size_t memOffset;
    int (*fptr)(void);  

    // Trap info for simulated system calls (set by runOperations)
    uint16_t trap_code;   // SI_READ / SI_WRITE / SI_SYS
    uint16_t trap_r0;     // sector or value
    uint16_t trap_r1;     // memory offset
    uint16_t trap_r2;     // count/length
} VirtualMachine;

VirtualMachine* createVM(HardDisk* disk, Memory* rmMemory, uint16_t fileId);
void destroyVM(VirtualMachine* vm);
void runVM(VirtualMachine* vm);
int runOperations(VirtualMachine* vm);
void custom_runVM(VirtualMachine* vm);
void writeProcessMemory(uint8_t memOffset, const void* val, VM_MEMORY* mem);
void* readProcessMemory(uint8_t cellOffset, const VM_MEMORY* mem);
void loadDemoProgram(void);
