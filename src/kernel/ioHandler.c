#include "ioHandler.h"
#include "resourceManager.h"
#include "../utils/utils.h"
#include "../../include/disk.h"
#include "../vm/vm.h"
#include <stdbool.h>

typedef struct {
    Process* proc;
    uint16_t code;
    uint16_t r0, r1, r2;
    int ticks;
    bool holdsRes;
} IORequest;

static IORequest pendingIO[8] = {0};
static int ioHead = 0;
static int ioTail = 0;

static void completeIO(IORequest* req, Scheduler* scheduler) {
    if (!req || !req->proc || !req->proc->vm) return;
    VirtualMachine* vm = req->proc->vm;
    uint8_t sectorBuf[DISK_SECTOR_SIZE] = {0};

    switch (req->code) {
        case SI_READ: {
            if (readDisk(&hardDisk, req->r0, sectorBuf)) {
                size_t bytes = req->r2 * 2;
                if (bytes > DISK_SECTOR_SIZE) bytes = DISK_SECTOR_SIZE;
                if (req->r1 + bytes > VM_MEMORY_SIZE) bytes = VM_MEMORY_SIZE - req->r1;
                memcpy(vm->memory->memoryCells + req->r1, sectorBuf, bytes);
                char buf[128];
                snprintf(buf, sizeof(buf), "[IO] READ done pid=%u sector=%u -> vm_addr=%u bytes=%zu\n",
                         req->proc->pid, req->r0, req->r1, bytes);
                _log(buf);
            }
            break;
        }
        case SI_WRITE: {
            size_t bytes = req->r2 * 2;
            if (bytes > DISK_SECTOR_SIZE) bytes = DISK_SECTOR_SIZE;
            if (req->r1 + bytes > VM_MEMORY_SIZE) bytes = VM_MEMORY_SIZE - req->r1;
            memcpy(sectorBuf, vm->memory->memoryCells + req->r1, bytes);
            writeDisk(&hardDisk, req->r0, sectorBuf);
            char buf[128];
            snprintf(buf, sizeof(buf), "[IO] WRITE done pid=%u vm_addr=%u -> sector=%u bytes=%zu\n",
                     req->proc->pid, req->r1, req->r0, bytes);
            _log(buf);
            break;
        }
        default:
            break;
    }

    if (req->holdsRes) {
        releaseResource(&resourceManager, 1, scheduler);
        req->holdsRes = false;
    }
    req->proc->state = READY;
    req->proc->waitingResId = 0;
    req->proc = NULL;
    req->code = 0;
}

bool enqueueIORequest(Process* proc, uint16_t code, uint16_t r0, uint16_t r1, uint16_t r2) {
    if (!proc) return false;

    // Claim IO resource (id=1) before enqueuing; if unavailable, the caller should retry later.
    if (!requestResource(&resourceManager, 1, proc)) {
        return false;
    }

    int next = (ioTail + 1) % 8;
    if (next == ioHead) {
        // queue full: release the resource and fail
        releaseResource(&resourceManager, 1, NULL);
        return false;
    }
    pendingIO[ioTail].proc = proc;
    pendingIO[ioTail].code = code;
    pendingIO[ioTail].r0 = r0;
    pendingIO[ioTail].r1 = r1;
    pendingIO[ioTail].r2 = r2;
    pendingIO[ioTail].ticks = 1;
    pendingIO[ioTail].holdsRes = true;
    ioTail = next;
    proc->state = BLOCKED;
    proc->waitingResId = 1;
    char buf[160];
    snprintf(buf, sizeof(buf), "[IO] queued pid=%u op=%u r0(sector)=%u r1(addr)=%u r2(words)=%u\n",
             proc->pid, code, r0, r1, r2);
    _log(buf);
    return true;
}

static void stepIO(Scheduler* scheduler) {
    if (ioHead == ioTail) return;
    IORequest* req = &pendingIO[ioHead];
    if (!req->proc || req->code == 0) {
        ioHead = (ioHead + 1) % 8;
        return;
    }
    if (req->ticks > 0) {
        req->ticks--;
        return;
    }
    completeIO(req, scheduler);
    req->proc = NULL;
    req->code = 0;
    ioHead = (ioHead + 1) % 8;
}

int ioHandlerEntry(Process* proc, Scheduler* scheduler, Kernel* kernel, uint16_t fileId) {
    (void)proc; (void)kernel; (void)fileId;
    if (!scheduler) return 0;

    // tick queued async I/O
    if (ioHead != ioTail) {
        IORequest* req = &pendingIO[ioHead];
        if (req && req->proc) {
            char buf[160];
            snprintf(buf, sizeof(buf), "[IO] ticking pid=%u op=%u ticks=%d\n",
                     req->proc->pid, req->code, req->ticks);
            _log(buf);
        }
        stepIO(scheduler);
    }

    // requests are enqueued by scheduler/interrupt handler
    return 1; // stay ready
}
