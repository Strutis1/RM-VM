// #pragma once

// #include "../../include/common.h"
// #include "../../include/instructions.h"
// #include "../../include/interrupts.h"
// #include "../../include/registers.h"
// #include "../../include/channel_device.h"

// Channel* initChannel();

// char writeChannel(Channel* channel);
// uint64_t* readChannel(Channel* channel);

#pragma once

#include "../../include/common.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/registers.h"
#include "../../include/channel_device.h"
#include "../../include/disk.h"
#include "../../include/memory.h"
#include <stdbool.h>
#include <stdint.h>

void VMinitChannel(ChannelDevice *channel);

bool VMchannelXCHG(ChannelDevice *channel, Memory *mem, HardDisk *disk, CPU *cpu, uint8_t src, uint8_t dst, uint16_t sb, uint16_t db, uint16_t count);

bool VMwriteChannel(ChannelDevice *channel, const uint8_t *data, uint16_t len);

bool VMreadChannel(ChannelDevice *channel, uint8_t *data, uint16_t len);
