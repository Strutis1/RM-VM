#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "scheduler.h"

#define MAX_RESOURCES 8
#define MAX_WAIT_PIDS 32

typedef struct {
    int id;
    char name[16];
    int available;
    unsigned char waitPids[MAX_WAIT_PIDS];
    int head;
    int tail;
} Resource;

typedef struct {
    Resource res[MAX_RESOURCES];
    int count;
} ResourceManager;

extern ResourceManager resourceManager;

void initResourceManager(ResourceManager* rm);
int createResource(ResourceManager* rm, int id, const char* name, int units);
bool requestResource(ResourceManager* rm, int id, Process* proc);
bool releaseResource(ResourceManager* rm, int id, Scheduler* sch);


#endif
