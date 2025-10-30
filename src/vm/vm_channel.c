// #include "vm_channel.h"

// Channel* initChannel() {

//     return NULL;
// }

// char writeChannel(Channel* channel) {

//     return 0;
// }

// uint64_t* readChannel(Channel* channel) {
    
//     return NULL;
// }
#include "vm_channel.h"
#include <stdio.h>
#include <string.h>


void VMinitChannel(ChannelDevice *channel) {
    if (!channel) return;
    channel->SB = channel->DB = 0;
    channel->ST = channel->DT = 0;
    channel->OFFSET = 0;
    channel->COUNT = 0;
    channel->DATA = 0;
    channel->busy = false;
}

bool VMchannelXCHG(ChannelDevice *channel, Memory *mem, HardDisk *disk, CPU *cpu,
                   uint8_t src, uint8_t dst, uint16_t sb, uint16_t db, uint16_t count) {
    if (!channel || !mem || !cpu) return false;

    channel->ST = src;
    channel->DT = dst;
    channel->SB = sb;
    channel->DB = db;
    channel->COUNT = count;
    channel->busy = true;

    printf("[VM:Channel] XCHG src=%d dst=%d sb=%d db=%d count=%d\n", src, dst, sb, db, count);

    // emulate the same XCHG logic as RM
    bool ok = channelXCHG(channel, mem, disk, cpu);
    if (!ok) {
        printf("[VM:Channel] XCHG failed\n");
        channel->busy = false;
        return false;
    }

    channel->busy = false;
    printf("[VM:Channel] XCHG complete\n");
    return true;
}

bool VMwriteChannel(ChannelDevice *channel, const uint8_t *data, uint16_t len) {
    if (!channel || !data || len == 0) return false;
    printf("[VM:Channel] Writing %u bytes via channel...\n", len);
    for (uint16_t i = 0; i < len; i++) {
        putchar(data[i]);
    }
    printf("\n[VM:Channel] Write complete.\n");
    return true;
}

bool VMreadChannel(ChannelDevice *channel, uint8_t *data, uint16_t len) {
    if (!channel || !data || len == 0) return false;
    printf("[VM:Channel] Reading %u bytes from input:\n", len);
    for (uint16_t i = 0; i < len; i++) {
        int ch = getchar();
        if (ch == EOF) return false;
        data[i] = (uint8_t)ch;
    }
    printf("[VM:Channel] Read complete.\n");
    return true;
}
