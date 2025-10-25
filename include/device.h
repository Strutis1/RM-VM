#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "channel.h"

/*
typedef struct {
    uint8_t id;
    uint8_t busy;
    uint8_t ready;
    uint8_t interruptFlag;
    void *data;
} Device;
*/

typedef struct {
    uint8_t id;
    Channel* chanPtr;
} Device;