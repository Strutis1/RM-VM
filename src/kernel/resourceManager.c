#include "resourceManager.h"
#include "scheduler.h"
#include "../utils/utils.h"
#include <string.h>

ResourceManager resourceManager = {0};

void initResourceManager(ResourceManager* rm) {
    if (!rm) return;
    memset(rm, 0, sizeof(ResourceManager));
}

static Resource* findRes(ResourceManager* rm, int id) {
    if (!rm) return NULL;
    for (int i = 0; i < rm->count; ++i) {
        if (rm->res[i].id == id) return &rm->res[i];
    }
    return NULL;
}

int createResource(ResourceManager* rm, int id, const char* name, int units) {
    if (!rm || rm->count >= MAX_RESOURCES) return -1;
    Resource* r = &rm->res[rm->count++];
    r->id = id;
    r->available = units < 0 ? 0 : units;
    strncpy(r->name, name ? name : "res", sizeof(r->name) - 1);
    r->name[sizeof(r->name) - 1] = '\0';
    r->head = r->tail = 0;
    return id;
}

static bool enqueue(Resource* r, unsigned char pid) {
    int next = (r->tail + 1) % MAX_WAIT_PIDS;
    if (next == r->head) return false; // full
    r->waitPids[r->tail] = pid;
    r->tail = next;
    return true;
}

static bool dequeue(Resource* r, unsigned char* pidOut) {
    if (r->head == r->tail) return false; // empty
    if (pidOut) *pidOut = r->waitPids[r->head];
    r->head = (r->head + 1) % MAX_WAIT_PIDS;
    return true;
}

bool requestResource(ResourceManager* rm, int id, Process* proc) {
    Resource* r = findRes(rm, id);
    if (!r || !proc) return false;
    if (r->available > 0) {
        r->available--;
        proc->waitingResId = 0;
        return true;
    }
    enqueue(r, proc->pid);
    proc->waitingResId = (uint8_t)id;
    proc->state = SUSP_BLOCKED;
    return false;
}

bool releaseResource(ResourceManager* rm, int id, Scheduler* sch) {
    Resource* r = findRes(rm, id);
    if (!r) return false;

    unsigned char pid = 0;
    if (dequeue(r, &pid)) {
        // wake that process
        if (sch) {
            for (int i = SCHEDULER_MIN_PRIORITY; i < SCHEDULER_MAX_PRIORITY; ++i) {
                Process* p = sch->schedule[i];
                if (p && p->pid == pid) {
                    p->state = READY;
                    p->waitingResId = 0;
                    break;
                }
            }
        }
    } else {
        r->available++;
    }
    return true;
}
