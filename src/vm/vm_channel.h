#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../include/common.h"
#include "../../include/memory.h"
#include "../../include/disk.h"
#include "../../include/registers.h"
#include "../../include/channel_device.h"

typedef struct {
    ChannelDevice dev;
    uint16_t dst_base;
    size_t last_qword_count;
    uint64_t *last_buffer;
} Channel;

Channel* initChannel(void);
char writeChannel(Channel* channel);
uint64_t* readChannel(Channel* channel);
