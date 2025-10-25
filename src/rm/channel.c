#include "../../include/channel.h"



Channel *createChannel(uint8_t id, Device *dev) {
    Channel *ch = (Channel*) malloc(sizeof(Channel));
    if (!ch) return NULL;

    ch->id = id;
    ch->busy = 0;
    ch->ready = 1;
    ch->interruptFlag = 0;
    ch->device = dev;
    return ch;
}

bool channelRead(Channel *ch, uint16_t addr, uint8_t *buffer) {
    if (!ch || !ch->device || ch->busy) return false;

    ch->busy = 1;
    bool ok = deviceRead(ch->device, addr, buffer);
    ch->busy = 0;
    ch->interruptFlag = ok;
    return ok;
}

bool channelWrite(Channel *ch, uint16_t addr, const uint8_t *buffer) {
    if (!ch || !ch->device || ch->busy) return false;

    ch->busy = 1;
    bool ok = deviceWrite(ch->device, addr, buffer);
    ch->busy = 0;
    ch->interruptFlag = ok;
    return ok;
}