#pragma once
#include "../../include/common.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/registers.h"

struct VirtualMachine;
typedef struct VirtualMachine VirtualMachine;

VM_CPU* initCPU(void);
void runOperations(VirtualMachine* vm);
