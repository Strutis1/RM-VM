#pragma once
// ============================================================
// Real Machine & Virtual Machine Architecture Constants
// ============================================================

// General configuration
#define WORD_SIZE          4
#define MEMORY_SIZE        256
#define PAGE_SIZE          16
#define PAGE_COUNT         (MEMORY_SIZE / PAGE_SIZE)
#define VM_MEMORY_SIZE     128
#define REG_COUNT          8

// Modes
#define MODE_USER          0
#define MODE_SUPERVISOR    1

// Interrupts
#define PI_NONE             0
#define PI_INVALID_OPCODE   1
#define PI_INVALID_ADDRESS  2
#define PI_OVERFLOW         3

#define SI_NONE             0
#define SI_READ             1
#define SI_WRITE            2
#define SI_HALT             3

#define TI_NONE             0
#define TI_EXPIRED          1

// Instructions
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
#define OP_PUSH     0x0B
#define OP_POP      0x0C
#define OP_READ     0x0D
#define OP_WRITE    0x0E

// Timer
#define TICK_LIMIT  10

// I/O
#define IO_CHANNELS   3
#define IO_CONSOLE    0
#define IO_DISK       1
#define IO_PRINTER    2

// Status
#define SUCCESS  0
#define FAILURE  1

#define TRUE     1
#define FALSE    0
