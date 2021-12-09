#include <stdio.h>
#include <stdbool.h>
#include "clock.h"
#include "CPU.h"
#include "memory.h"
#include "imemory.h"
#include "cache.h"

void clk_reset(Clock* clk){ clk->count = 0; }

void clk_loop(Memory* mem, IMemory* imem, CPU* cpu, Cache* cache){
	bool workToDo = true;
	memStartTick(mem);
	cpuStartTick(cpu, mem, cache);
	while(workToDo){
		memDoCycleWork(mem, cache);
		cpuDoCycleWork(cpu, mem, imem, cache);
		workToDo = cpuIsMoreCycleWorkNeeded(cpu) || memIsMoreCycleWorkNeeded(mem);
	}
}

void tick(Clock* clk, Memory* mem, IMemory* imem, CPU* cpu, Cache* cache, int ticks){
	for(int i = 0; i < ticks; i++){
		clk_loop(mem, imem, cpu, cache);
		clk->count++;
	}
}

void clk_dump(Clock* clk){ printf("Clock: %d\n", clk->count); }
