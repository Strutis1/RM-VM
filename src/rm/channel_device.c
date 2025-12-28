#include "../../include/channel_device.h"
#include "../../include/disk.h"
#include "../utils/utils.h"

#include <stdio.h>

void initChannelDevice(ChannelDevice *channel) {
    if (!channel) return;
    channel->SB = channel->DB = 0;
    channel->ST = channel->DT = 0;
    channel->OFFSET = 0;
    channel->COUNT = 0;
    channel->DATA = 0;
    channel->busy = false;
}

void resetChannelDevice(ChannelDevice *channel) {
    if (!channel) return;
    channel->busy = false;
    channel->SB = channel->DB = 0;
    channel->ST = channel->DT = 0;
    channel->OFFSET = 0;
    channel->COUNT = 0;
    channel->DATA = 0;
}

bool channelXCHG(ChannelDevice *channel, Memory *mem, HardDisk *disk, CPU *cpu) {
    if (!channel || !mem) return false;
    channel->busy = true;

    char buf[128];
    snprintf(buf, sizeof(buf),
             "[Channel] XCHG start: ST=%d SB=%d -> DT=%d DB=%d (%d words)\n",
             channel->ST, channel->SB, channel->DT, channel->DB, channel->COUNT);
    _log(buf);

    switch (channel->ST) {
        case CH_SRC_USER_MEM:
            if (channel->DT == CH_DST_SUPER_MEM) {
                for (uint16_t i = 0; i < channel->COUNT; ++i)
                    writeSupervisor(mem, channel->DB + i, readUser(mem, channel->SB + i));
            } else if (channel->DT == CH_DST_IO) {
                for (uint16_t i = 0; i < channel->COUNT; ++i) {
                    snprintf(buf, sizeof(buf), "[I/O OUT] %04X\n", readUser(mem, channel->SB + i));
                    _log(buf);
                }
                cpu->SI = SI_WRITE;
            }
            break;

        case CH_SRC_SUPER_MEM:
            if (channel->DT == CH_DST_USER_MEM) {
                for (uint16_t i = 0; i < channel->COUNT; ++i)
                    writeUser(mem, channel->DB + i, readSupervisor(mem, channel->SB + i));
            }
            break;

        case CH_SRC_DISK:
            if (channel->DT == CH_DST_SUPER_MEM) {
                for (uint16_t i = 0; i < channel->COUNT; ++i)
                    writeSupervisor(mem, channel->DB + i, diskReadWord(disk, channel->SB + i));
                cpu->SI = SI_READ;
            }
            break;

        case CH_SRC_IO:
            if (channel->DT == CH_DST_CPU_REG) {
                _log("[I/O IN] Enter value (hex): ");
                scanf("%hx", &cpu->R[0]);
                cpu->SI = SI_READ;
            }
            break;

        default:
            _log("[Channel] Invalid source type!\n");
            cpu->PI = PI_INVALID_ADDRESS;
            channel->busy = false;
            return false;
    }

    channel->busy = false;
    _log("[Channel] XCHG complete.\n");
    return true;
}

void printChannelState(const ChannelDevice *channel) {
    if (!channel) return;
    char buf[128];
    snprintf(buf, sizeof(buf),
             "[Channel] ST=%d DT=%d SB=%d DB=%d COUNT=%d OFFSET=%d\n",
             channel->ST, channel->DT, channel->SB, channel->DB,
             channel->COUNT, channel->OFFSET);
    _log(buf);
}
