#include "../../include/channel.h"
#include "../utils/utils.h"

char writeChannelData(Channel* chan, uint64_t* binPtr) {
    if (chan->busy == 1) {
        do {
            sleep_ms(500);
        } while (chan->busy == 1);
    }
    chan->busy = 1;
    chan->flag = 0;
    chan->bin = (uint64_t*) malloc(sizeof(binPtr) / sizeof(uint64_t));
    chan->busy = 1;

    return 0;
}

//return up to 4 bytes as the answes can be up to 4 bytters
uint32_t readChannelData(Channel* chan) { 
    chan->busy = 1;
    chan->flag = 1;
    uint32_t ans = (uint32_t)(chan->bin[0] & 0xFFFFFFFFULL);
    chan->busy = 0;
    return ans;
}