#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../../include/registers.h"
#include "../../include/interrupts.h"
#include "../../include/memory.h"

#define MMU_PAGE_SIZE   256
#define MMU_PAGE_COUNT  256
#define INVALID_PAGE    0xFFFF

typedef struct {
    uint16_t frame;   // num
    bool present;
    bool writable;
} PageTableEntry;

uint16_t mmuTranslate(uint16_t virtualAddr, CPU* cpu, Memory* mem);
void initPageTable(CPU* cpu, Memory* mem, uint16_t ptAddr);
