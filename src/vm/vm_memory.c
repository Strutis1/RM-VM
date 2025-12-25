#include "vm_memory.h"

#include <string.h>
#include <memory.h>
#include "../../include/common.h"
#include "../../include/disk.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"


VM_MEMORY* VMinitMemory() {
    // VM_MEMORY* abc;
    // memset(abc->memoryCells, 0x00, VM_MEMORY_SIZE);
    // return abc;
    VM_MEMORY* mem = malloc(sizeof(VM_MEMORY));
    if (!mem) return NULL;
    memset(mem->memoryCells, 0, sizeof(mem->memoryCells));
    return mem;
}

char loadProgram(Instruction* ins, VM_MEMORY* mem) {
    size_t pc = 0;
    for (int i = 0;; ++i) {
        if (pc + 9 > VM_MEMORY_SIZE) return 1;
        mem->memoryCells[pc + 0] = ins[i].opcode;
        mem->memoryCells[pc + 1] = ins[i].regA;
        mem->memoryCells[pc + 2] = ins[i].regB;
        mem->memoryCells[pc + 3] = ins[i].mode;
        mem->memoryCells[pc + 4] = (uint8_t)(ins[i].operand >> 8);
        mem->memoryCells[pc + 5] = (uint8_t)(ins[i].operand & 0xFF);
        mem->memoryCells[pc + 6] = (uint8_t)(ins[i].raw >> 8);
        mem->memoryCells[pc + 7] = (uint8_t)(ins[i].raw & 0xFF);
        mem->memoryCells[pc + 8] = ins[i].length;
        pc += 9;
        if (ins[i].opcode == OP_HALT) break;
    }
    return 0;
}

static inline char stuffInstruction(Instruction *ins, uint16_t word) {
    if (!ins) return 5;

    ins->raw     = word;
    ins->opcode  = (word >> 12) & 0xF;
    if (ins->opcode > 0xE) return 1;

    ins->regA    = (word >> 9) & 0x7;
    if (ins->regA >= REG_COUNT) return 2;

    ins->regB    = (word >> 6) & 0x7;
    if (ins->regB >= REG_COUNT) return 3;

    ins->mode    = (word >> 4) & 0x3;
    if (ins->mode > 3) return 4;

    ins->operand =  word        & 0xF;
    ins->length  = 1;

    return 0;
}