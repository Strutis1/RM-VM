#pragma once

#include <stdint.h>
#include <stdbool.h>

void IOinit(void);
bool IOwrite(uint16_t sector, uint16_t srcAddr, uint16_t count);
bool IOread(uint16_t sector, uint16_t destAddr, uint16_t count);
void IOcheckInterrupts(void);
