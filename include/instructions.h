#pragma once




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

