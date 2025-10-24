#pragma once



// General configuration
#define WORD_SIZE          4
#define REG_COUNT          8


// Memory
#define MEMORY_SIZE        512
#define PAGE_SIZE          16
#define PAGE_COUNT         (MEMORY_SIZE / PAGE_SIZE)
#define USER_MEMORY_START 0
#define USER_MEMORY_END 255
#define SUPERVISOR_MEMORY_START 256
#define SUPERVISOR_MEMORY_END 511
#define PTR_START          240 //for now


// Modes
#define MODE_USER          0x0
#define MODE_SUPERVISOR    0x1

// Timer
#define TICK_LIMIT  10

// Status
#define SUCCESS  0x1
#define FAILURE  0x0

// Flags
#define FLAG_ZERO      0x1
#define FLAG_NEGATIVE  0x2
#define FLAG_OVERFLOW  0x4
#define FLAG_CARRY     0x8


// Program
#define MAX_PROGRAM_SIZE VM_MEMORY_SIZE // temp
#define MIN_PROGRAM_SIZE 32 // temp
    // will mark start and end of the program 
#define MAGIC_BYTES 0xAB 0xCD 0xEF // magic bytes
#define MAGIC_SEPERATOR 0xFE 0xDC 0xBA // magic byte end
#define EMPTY 0x00

// Misc
// #define NULL 0LL defined somewhere else
