#pragma once

#include <stdint.h>
#include <stdbool.h>

void IOinit(void);
bool IOread(uint16_t sector, uint8_t *buffer);
bool IOwrite(uint16_t sector, const uint8_t *buffer);
void IOcheckInterrupts(void);
