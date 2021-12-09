#include <stdbool.h>
#include "memory.h"
#include "imemory.h"
#include "cache.h"
#ifndef _CPU_
#define _CPU_

//State definitions
#define IDLE 0
#define WAIT 1
#define FETCH 2
#define DECODE 3
#define MEMREQ 4

typedef struct {
	unsigned int regs[8], pc, tc, state;
	bool halted;
} CPU;

typedef struct {
	unsigned int type, dest, src, trg, val;
} dec_inst;

void cpu_reset(CPU* cpu);

void cpu_set(CPU* cpu, char* regName, int value);

void cpu_dump(CPU* cpu);

void decode(CPU* cpu, Memory* mem, IMemory* imem, Cache* cache);

void cpuStartTick(CPU* cpu, Memory* mem, Cache* cache);

void cpuDoCycleWork(CPU* cpu, Memory* mem, IMemory* imem, Cache* cache);

bool cpuIsMoreCycleWorkNeeded(CPU* cpu);

//Old, only used in Assignment 1
void cpu_tick(CPU* cpu);

#endif
