#pragma once


#include "../../include/common.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/channel_device.h"


typedef struct {
    uint8_t memoryCells[VM_MEMORY_SIZE];
} VM_MEMORY;


VM_MEMORY* VMinitMemory();


static char stuffInstruction(Instruction *ins, uint16_t word);
char loadProgram(Instruction* ins, VM_MEMORY* mem);