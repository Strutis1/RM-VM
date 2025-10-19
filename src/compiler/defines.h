#pragma once
#include "defines.h"
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

// ripped from the main program
#define DATASEG_SIZE 96
#define VM_MEMORY_SIZE     128
#define MAX_PROGRAM_SIZE VM_MEMORY_SIZE // temp
#define MIN_PROGRAM_SIZE 32 // temp
#define ANSWER_REGION 32


    // will mark start and end of the program 
#define MAGIC_BYTES 0xAB 0xCD 0xEF // magic bytes
#define MAGIC_SEPERATOR 0xFE 0xDC 0xBA // magic byte end
#define EMPTY 0x00

// Misc
#define NULL 0LL

/*
typedef struct {
    uint16_t R[REG_COUNT - 2];   //??
    uint16_t PC;     
    uint16_t SF;     
    uint16_t DS;    
    uint16_t CS;
    uint16_t PTR;
    uint8_t SI;    
} VM_CPU;
*/

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

typedef struct {
    Instruction opps[MAX_PROGRAM_SIZE];
} Program;