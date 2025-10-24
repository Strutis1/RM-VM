#pragma once


#include "../../include/common.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/channel.h"
#include "../../include/device.h"
#include "../../include/registers.h"



#include "vm_cpu.h"
#include "vm_memory.h"
#include "vm_program.h"

typedef struct {
    Instruction* instructions;
    Channel* channel;
    VM_MEMORY* memory;
    VM_CPU* vm_cpu;
} VirtualMachine;



VirtualMachine* createVM(Channel* channel);