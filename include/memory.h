#pragma once


#define MEMORY_SIZE        256
#define PAGE_SIZE          16
#define PAGE_COUNT         (MEMORY_SIZE / PAGE_SIZE)
#define VM_MEMORY_SIZE     128
#define PTR_START          240 //for now


typedef struct{
    int cells[MEMORY_SIZE];
} Memory;


extern Memory physicalMemory;



void write(Memory *mem, int address, int value);
int read(Memory *mem, int address);
