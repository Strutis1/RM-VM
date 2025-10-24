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
        if (pc + 8 > VM_MEMORY_SIZE) return 1;
        uint64_t w = 0;
        w |= ((uint64_t)(ins[i].opcode & 0xFF))   << 56;
        w |= ((uint64_t)(ins[i].reg    & 0xFF))   << 48;
        w |= ((uint64_t)(ins[i].mode   & 0xFF))   << 40;
        w |= ((uint64_t)(ins[i].operand& 0xFFFF)) << 24;
        w |= ((uint64_t)(ins[i].length & 0xFF))   << 16;
        w |= ((uint64_t)(ins[i].raw    & 0xFFFF));
        for (int b = 0; b < 8; ++b)
            mem->memoryCells[pc + b] = (uint8_t)((w >> ((7 - b) * 8)) & 0xFF);
        pc += 8;
        if (ins[i].opcode == OP_HALT) break;
    }
    return 0;
}
