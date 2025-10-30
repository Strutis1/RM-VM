#pragma once
#include "../../include/common.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/registers.h"

#include "vm_cpu.h"
#include "vm_memory.h"
#include "vm_channel.h"

typedef struct VirtualMachine {
    Channel*   channel;
    VM_MEMORY* memory;
    VM_CPU*    vm_cpu;
} VirtualMachine;

VirtualMachine* createVM(Channel* channel);