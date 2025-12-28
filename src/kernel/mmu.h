#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../../include/registers.h"
#include "../../include/interrupts.h"
#include "../../include/memory.h"

#define PAGE_SIZE      256
#define PAGE_COUNT     256
#define INVALID_PAGE   0xFFFF

typedef struct {
    uint16_t frame;   // num
    bool present;
    bool writable;
} PageTableEntry;
