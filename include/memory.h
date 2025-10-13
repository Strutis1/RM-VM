#pragma once

#include <stdint.h>

#include "common.h"


typedef struct{
    uint16_t cells[MEMORY_SIZE];
} Memory;


extern Memory physicalMemory;

void initMemory(Memory *mem);

void write(Memory *mem, uint16_t address, uint16_t value);
uint16_t read(Memory *mem, uint16_t address);
