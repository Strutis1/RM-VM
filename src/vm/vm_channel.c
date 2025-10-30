#include "vm_channel.h"
#include <stdlib.h>
#include <string.h>

static inline void free_last(Channel* c) {
    if(c->last_buffer) {
        free(c->last_buffer);
        c->last_buffer = NULL;
        c->last_qword_count = 0;
    }
}

Channel* initChannel(void) {
    Channel* ch = (Channel*)calloc(1, sizeof(Channel));
    if(!ch) return NULL;
    initChannelDevice(&ch->dev);

    ch->dst_base = SUPERVISOR_MEMORY_START;
    ch->last_qword_count = 0;
    ch->last_buffer = NULL;

    return ch;
}

uint64_t* readChannel(Channel* channel) {
    if(!channel) return NULL;

    free_last(channel);

    channel->dev.SB = 0;
    channel->dev.DB = channel->dst_base;
    channel->dev.ST = CH_SRC_DISK;
    channel->dev.DT = CH_DST_SUPER_MEM;
    channel->dev.OFFSET = 0;
    channel->dev.COUNT = DISK_SECTOR_SIZE;

    if(!channelXCHG(&channel->dev, &physicalMemory, &hardDisk, &realCPU)) return NULL;

    uint8_t tmp[DISK_SECTOR_SIZE];
    for(uint16_t i = 0;i<DISK_SECTOR_SIZE;i++) tmp[i] = (uint8_t)(read(&physicalMemory, (uint16_t)(channel->dst_base+i))&0xFF);

    uint64_t* out = (uint64_t*)malloc((DISK_SECTOR_SIZE/8)*sizeof(uint64_t));

    if(!out) return NULL;

    memcpy(out, tmp, DISK_SECTOR_SIZE);

    channel->last_buffer = out;
    channel->last_qword_count = DISK_SECTOR_SIZE/8;

    return out;
}

char writeChannel(Channel* channel) {
    if(!channel) return 1;
    if(!channel->last_buffer) return 2;

    for(uint16_t i = 0;i<DISK_SECTOR_SIZE;i++)
        write(&physicalMemory, (uint16_t)(channel->dst_base+i), ((uint16_t*)channel->last_buffer)[i]&0xFF);

    channel->dev.SB = channel->dst_base;
    channel->dev.DB = 0;
    channel->dev.ST = CH_SRC_SUPER_MEM;
    channel->dev.DT = CH_DST_DISK;
    channel->dev.OFFSET = 0;
    channel->dev.COUNT = DISK_SECTOR_SIZE;
    
    return channelXCHG(&channel->dev, &physicalMemory, &hardDisk, &realCPU)?0:3;
}
