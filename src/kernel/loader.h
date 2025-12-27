#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "../../include/memory.h"
#include "../../include/registers.h"
#include "../../include/disk.h"
#include "../../include/common.h"

// Program image header stored at the beginning of the first sector.
typedef struct {
    uint16_t wordCount;   // total 16-bit words in the image (excluding header)
    uint16_t entryOffset; // offset (in words) from loadAddress to the entry point
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
