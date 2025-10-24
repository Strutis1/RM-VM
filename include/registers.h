#pragma once

#include "common.h"
#include <stdint.h>


typedef struct {
    uint16_t IC;
    uint16_t R[REG_COUNT];  
    uint16_t SF;
    uint16_t MODE;
    uint16_t PI;
    uint16_t SI;
    uint16_t TI;
    uint16_t PTR;
} CPU;

extern CPU realCPU;



typedef struct {
    uint16_t R[REG_COUNT];   
    uint16_t PC;     
    uint16_t SF;     
    uint16_t DS;    
    uint16_t CS;
    uint16_t PTR;
    uint8_t SI;    
} VM_CPU;


extern VM_CPU vmCPU;//one for now

