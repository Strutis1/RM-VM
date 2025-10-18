#pragma once
// General configuration
#define WORD_SIZE          4
#define REG_COUNT          8


// Memory
#define MEMORY_SIZE        256
#define PAGE_SIZE          16
#define PAGE_COUNT         (MEMORY_SIZE / PAGE_SIZE)
#define VM_MEMORY_SIZE     128
#define PTR_START          240 //for now

// Disk
#define DISK_SECTOR_SIZE 16
#define DISK_SECTOR_COUNT 64


// Modes
#define MODE_USER          0
#define MODE_SUPERVISOR    1

// Timer
#define TICK_LIMIT  10

// Status
#define SUCCESS  0
#define FAILURE  1

// Program
#define MAX_PROGRAM_SIZE 256 // temp
#define MIN_PROGRAM_SIZE 32 // temp
    // will mark start and end of the program 
#define MAGIC_BYTES 0xAB 0xCD 0xEF // magic bytes
#define MAGIC_SEPERATOR 0xFE 0xDC 0xBA // magic byte end
#define EMPTY 0x00

// Misc
#define NULL 0LL