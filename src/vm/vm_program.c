#include "vm_program.h"

/*
#define PC          0x01
#define SF          0x02
#define DS          0x03
#define CS          0x04
#define PTR         0x05
#define SI          0x06

#define OP_HALT     0x00
#define OP_LOAD     0x01
#define OP_STORE    0x02
#define OP_ADD      0x03
#define OP_SUB      0x04
#define OP_MUL      0x05
#define OP_DIV      0x06
#define OP_CMP      0x07
#define OP_JMP      0x08
#define OP_JZ       0x09
#define OP_JNZ      0x0A
// #define OP_PUSH     0x0B
// #define OP_POP      0x0C
#define OP_READ     0x0D
#define OP_WRITE    0x0E

#define OP_NOP      0x0F   // no operation (optional placeholder)
#define OP_SYS      0x10   // reserved for system call / interrupt


typedef struct {
    uint8_t opcode;
    uint8_t reg;
    uint8_t mode;
    uint16_t operand;
    uint8_t length;
    uint16_t raw;
} Instruction;
*/

char stuffInstructions(Instruction* ins, const uint64_t insRegion) {
    ins->opcode  = (insRegion >> 56) & 0xFF;
    if (ins->opcode > 0x10) return 1;

    ins->reg     = (insRegion >> 48) & 0xFF;
    if (ins->reg > 0x06) return 2;

    ins->mode    = (insRegion >> 40) & 0xFF;
    if (ins->mode > 0x04) return 3;

    ins->operand = (insRegion >> 24) & 0xFFFF;
    if (ins->operand > 0xFF) return 4;
    
    ins->length  = (insRegion >> 16) & 0xFF;
    ins->raw     = insRegion & 0xFFFF;

    return 0;
}