#pragma once

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
// =======
// #include "../../include/common.h"
// #include "../../include/instructions.h"
// #include "../../include/interrupts.h"
// >>>>>>> 4104e68847041b9d8d6f1b8c3de28571f2adc05c
#include "../../include/registers.h"
#include <stdlib.h>
#include <string.h>

// Forward declaration only, not a typedef redefinition
struct VirtualMachine;
//typedef struct VirtualMachine VirtualMachine;

VM_CPU* initVMCPU(void);
void runOperations(struct VirtualMachine* vm);
// =======
// struct VirtualMachine;
// typedef struct VirtualMachine VirtualMachine;

// VM_CPU* initCPU(void);
// void runOperations(VirtualMachine* vm);
// >>>>>>> 4104e68847041b9d8d6f1b8c3de28571f2adc05c
