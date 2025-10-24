#include "vm_memory.h"

#include <memory.h>
#include "../../include/common.h"
#include "../../include/disk.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"


VM_MEMORY* VMinitMemory() {
    VM_MEMORY* abc;
    memset(abc->memoryCells, 0x00, VM_MEMORY_SIZE);
    return abc;
}

/*
typedef struct {
    uint8_t opcode;
    uint8_t reg;
    uint8_t mode;
    uint16_t operand;
    uint8_t length;
    uint16_t raw;
} Instruction;
*/

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

char stuffInstructions(Instruction* ins, const uint64_t insRegion) {
    ins->opcode  = (insRegion >> 56) & 0xFF;
    if (ins->opcode > 0x10) return 1;

    ins->regA    = (insRegion >> 48) & 0xFF;
    if (ins->regA > 0x06) return 2;

    ins->regB    = (insRegion >> 40) & 0xFF;
    if (ins->regB > 0x06) return 5;

    ins->mode    = (insRegion >> 32) & 0xFF;
    if (ins->mode > 0x04) return 3;

    ins->operand = (insRegion >> 16) & 0xFFFF;
    ins->raw     = insRegion & 0xFFFF;
    ins->length  = 8;

    return 0;
}
