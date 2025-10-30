#pragma once

#include <stdint.h>

#include "common.h"


typedef struct{
    uint16_t cells[MEMORY_SIZE];
} Memory;


extern Memory physicalMemory;

void RMinitMemory(Memory *mem);

void write(Memory *mem, uint16_t address, uint16_t value);
uint16_t read(Memory *mem, uint16_t address);

uint16_t readUser(Memory *mem, uint16_t addr);
void writeUser(Memory *mem, uint16_t addr, uint16_t value);

uint16_t readSupervisor(Memory *mem, uint16_t addr);
void writeSupervisor(Memory *mem, uint16_t addr, uint16_t value);
