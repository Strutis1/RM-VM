#pragma once

typedef struct VirtualMachine VirtualMachine;

/*
typedef struct {
    uint16_t R[REG_COUNT];   
    uint16_t PC;     
    uint16_t SF;     
    uint16_t DS;    
    uint16_t CS;     
    uint16_t PTR;    
} VM_CPU;
*/

#include "../../include/common.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/channel_device.h"
#include "../../include/registers.h"
#include <stdlib.h>
#include <string.h>

// Forward declaration only, not a typedef redefinition
struct VirtualMachine;
//typedef struct VirtualMachine VirtualMachine;

VM_CPU* initVM_CPU();
void runOperations(VirtualMachine* vm);
void runInstruction(VirtualMachine* vm); // dont delete this, it will replace runOperations