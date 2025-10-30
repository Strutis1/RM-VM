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
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../include/common.h"
#include "../../include/memory.h"
#include "../../include/disk.h"
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
// =======

// typedef struct {
//     ChannelDevice dev;
//     uint16_t dst_base;
//     size_t last_qword_count;
//     uint64_t *last_buffer;
// } Channel;

// Channel* initChannel(void);
// char writeChannel(Channel* channel);
// uint64_t* readChannel(Channel* channel);
// >>>>>>> 4104e68847041b9d8d6f1b8c3de28571f2adc05c
