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


//changed the function because of changed instruction structure
// changed the name from stuffInstruction to execute... 

char executeInstruction(Instruction* ins, const uint16_t insWord) {
    ins->raw = insWord;

    ins->opcode = (insWord >> 12) & 0xF;
    if (ins->opcode > 0xE) return 1; 

    ins->regA = (insWord >> 9) & 0x7;
    if (ins->regA >= REG_COUNT) return 2; 

    ins->mode = 0;
    ins->regB = 0;
    ins->operand = 0;
    ins->length = 1; 

    switch (ins->opcode) {
        case OP_LOAD:
        case OP_STORE:
            ins->mode = (insWord >> 7) & 0x3;
            ins->operand = insWord & 0x7F;
            if (ins->mode > 3) return 3;
            break;

        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_CMP:
            ins->regB = (insWord >> 6) & 0x7;
            if (ins->regB >= REG_COUNT) return 4;
            break;

        case OP_JMP:
        case OP_JZ:
        case OP_JNZ:
            ins->operand = insWord & 0x1FF;
            break;

        case OP_READ:
        case OP_WRITE:
        case OP_HALT:
        case OP_NOP:
        case OP_SYS:
            break;

        default:
            return 5; 
    }

    return 0; 
}



/*
typedef struct {
    uint16_t R[REG_COUNT];   
    uint16_t PC;     
    uint16_t SF;     
    uint16_t DS;    
    uint16_t CS;
    uint16_t PTR;
    uint8_t SI;    
} VM_CPU;
*/