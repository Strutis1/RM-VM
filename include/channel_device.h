#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "memory.h"
#include "disk.h"
#include "interrupts.h"
#include "../src/rm/cpu.h"

#define CH_SRC_USER_MEM    1
#define CH_SRC_SUPER_MEM   2
#define CH_SRC_DISK        3
#define CH_SRC_IO          4
#define CH_SRC_CPU_REG     5

#define CH_DST_USER_MEM    10
#define CH_DST_SUPER_MEM   11
#define CH_DST_DISK        12
#define CH_DST_IO          13
#define CH_DST_CPU_REG     14

typedef struct {
    uint16_t SB;
    uint16_t DB;
    uint16_t ST;
    uint16_t DT;
    uint16_t OFFSET;
    uint16_t COUNT;
    uint16_t DATA;
    bool busy;
} ChannelDevice;

void initChannelDevice(ChannelDevice *channel);
void resetChannelDevice(ChannelDevice *channel);
bool channelXCHG(ChannelDevice *channel, Memory *mem, HardDisk *disk, CPU *cpu);
void printChannelState(const ChannelDevice *channel);
