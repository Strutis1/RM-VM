#pragma once


#include "../../include/common.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/channel.h"
#include "../../include/device.h"


typedef struct {
    uint8_t memoryCells[VM_MEMORY_SIZE];
} VM_MEMORY;


VM_MEMORY* VMinitMemory();
char loadProgram(Instruction* ins, VM_MEMORY* mem);