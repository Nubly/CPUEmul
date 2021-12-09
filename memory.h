#include <stdbool.h>
#include "cache.h"
#ifndef MEMORY
#define MEMORY

//State definitions
#define IDLE 0
#define WAIT 1
#define READ 2
#define WRITE 3
#define CACHE_READ 4
#define CACHE_WRITE 5

typedef struct { 
	unsigned int size, addr, count, state, ctr;
	unsigned int* data;
	unsigned int* trgPtr;
} Memory;

void mem_create(Memory* mem, int size);

void mem_reset(Memory* mem);

void mem_dump(Memory* mem, int addr, int count);

void mem_set(Memory* mem, int addr, int count, int* data);

void memStartTick(Memory* mem);

void memDoCycleWork(Memory* mem, Cache* cache);

bool memIsMoreCycleWorkNeeded(Memory* mem);

void memStartFetch(Memory* mem, unsigned int addr, unsigned int* trgPtr);

void memStartStore(Memory* mem, unsigned int addr, unsigned int* trgPtr);

#endif
