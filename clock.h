#include "CPU.h"
#include "memory.h"
#include "imemory.h"
#include "cache.h"
#ifndef CLOCK
#define CLOCK

typedef struct {
	int count;
} Clock;

void clk_reset(Clock* clk);

void clk_loop(Memory* mem, IMemory* imem, CPU* cpu, Cache* cache);

//Old, only used in Assignment 1
void tick(Clock* clk, Memory* mem, IMemory* imem, CPU* cpu, Cache* cache, int ticks);

void clk_dump(Clock* clk);

#endif
