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
#include "../../include/registers.h"

// implement the instructions

VM_CPU* initCPU();

void runOperations(VirtualMachine* vm);