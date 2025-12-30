#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "../../include/memory.h"
#include "../../include/registers.h"
#include "../../include/disk.h"
#include "../../include/common.h"

#include "../kernel/scheduler.h"
#include "kernel.h"
#include "../vm/vm.h"

// Program image header stored at the beginning of the first sector.
typedef struct {
    uint16_t wordCount;    // total 16-bit words in the image (excluding header)
    uint16_t entryOffset;  // offset (in words) from loadAddress to the entry point
    uint16_t loadAddress;  // base address in VM user memory where code will be placed
} ProgramHeader;

typedef struct {
    uint16_t loadAddress; // base address in user memory where the program was placed
    uint16_t entryPoint;  // absolute address where execution should start
    uint16_t wordsLoaded; // number of words copied into memory
    uint16_t startSector; // disk sector the loader started from
    uint16_t sectorsUsed; // number of sectors consumed
} LoadResult;

// Load a program image from disk into user memory.
// Image format: first sector begins with ProgramHeader, followed by raw 16-bit words
// laid out little-endian. Subsequent sectors continue the word stream.
// Returns true on success and optionally fills LoadResult. On success, cpu->IC
// is set to the computed entry point (loadAddress + entryOffset).
bool loadProgramImage(HardDisk *disk, Memory *mem, CPU *cpu, uint16_t startSector, uint16_t loadAddress, LoadResult *out);

//file table for mapping FileID -> disk sectors.
#define MAX_FILE_ENTRIES 8
typedef struct {
    uint16_t fileId;
    uint16_t startSector;
    uint16_t sectorCount;
} FileEntry;

void initFileTable(void);
bool registerFile(uint16_t fileId, uint16_t startSector, uint16_t sectorCount);
bool findFile(uint16_t fileId, FileEntry *out);

//Read a file by FileID into a caller-provided buffer
//Returns true on success and fills outBytes with bytes copied
bool readFileToBuffer(HardDisk *disk, uint16_t fileId, uint8_t *dst, size_t maxBytes, size_t *outBytes);

//Loader routine: load a FileID into a VM's memory and unblock waiting
bool loaderLoadFile(HardDisk *disk, VirtualMachine *vm, uint16_t fileId, Scheduler *sch, unsigned char waiterPid);

// Loader process entry (for Scheduler-managed processes)
int loaderEntry(Process* proc, Scheduler* scheduler, Kernel* kernel, uint16_t fileId);
