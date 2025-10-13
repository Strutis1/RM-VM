#pragma once

#include <stdint.h>

#define MEMORY_SIZE        256
#define PAGE_SIZE          16
#define PAGE_COUNT         (MEMORY_SIZE / PAGE_SIZE)
#define VM_MEMORY_SIZE     128
#define PTR_START          240 //for now


typedef struct{
    uint16_t cells[MEMORY_SIZE];
} Memory;


extern Memory physicalMemory;



void write(Memory *mem, uint16_t address, uint16_t value);
uint16_t read(Memory *mem, uint16_t address);
