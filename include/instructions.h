#pragma once
#include <stdint.h>


#define OP_HALT     0x0
#define OP_LOAD     0x1
#define OP_STORE    0x2
#define OP_ADD      0x3
#define OP_SUB      0x4
#define OP_MUL      0x5
#define OP_DIV      0x6
#define OP_CMP      0x7
#define OP_JMP      0x8
#define OP_JZ       0x9
#define OP_JNZ      0xA
#define OP_READ     0xB
#define OP_WRITE    0xC
#define OP_NOP      0xD
#define OP_SYS      0xE 



typedef struct {
    uint8_t opcode;     
    uint8_t regA;       
    uint8_t regB;       
    uint8_t mode;       
    uint16_t operand;   
    uint16_t raw;       
    uint8_t length;     
} Instruction;


