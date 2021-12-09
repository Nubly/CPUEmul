#include <stdio.h>
#include <stdbool.h>
#include "CPU.h"
#include "memory.h"
#include "imemory.h"
#include "cache.h"

void cpu_reset(CPU* cpu){
	cpu->pc = cpu->tc = 0;
	for(int i = 0; i < 8; i++)
		cpu->regs[i] = 0;
}

void cpu_set(CPU* cpu, char* regName, int value){
	if(regName[0] == 'P') cpu->pc = value; 
	//Cast to int using ascii code, because why not?
	else cpu->regs[(int)regName[1] - 65] = value;
}

void cpu_dump(CPU* cpu){
	printf("PC: 0x%02X\n", cpu->pc);
	for(int i = 0; i < 8; i++)
		//Invert int cast from set to turn int back to regName
		printf("R%c: 0x%02X\n", (char)i + 65, cpu->regs[i]);
	printf("TC: %d\n\n", cpu->tc);
}

void decode(CPU* cpu, Memory* mem, IMemory* imem, Cache* cache){
	//Instruction is first 3 bits, then dest, then src, then trg, then immediate for rest
	dec_inst x;
	unsigned int inst = imem->data[cpu->pc];
	x.type = inst >> 17;
	x.dest = inst >> 14 & 0b111;
	x.src = inst >> 11 & 0b111;
	x.trg = inst >> 8 & 0b111;
	x.val = inst & 0b11111111;

	//Load word instruction - 1 cycle or speed of mem
	if(x.type == 0b101){
		//Cache enabled
		if(cache->enabled){
			
			cache->calculated = cpu->regs[x.trg]/8;
			unsigned int rem = cpu->regs[x.trg] % 8;

			//Invalidate cache data
			if(x.dest == 0xFF){
				cpu->regs[x.dest] = 0;
				for(int i = 0; i < 8; i++)
					cache->states[i] = 'I';
			}

			//Cache hit
			else if(cache->clo == cache->calculated && cache->states[rem] == 'V')
				cpu->regs[x.dest] = cache->data[rem];

			//Cache miss, request new data from mem
			else{
				cache->clo = cache->calculated;
				mem->state = CACHE_READ;
			}

			cpu->state = WAIT;
			cpu->pc++;
		}
		//Cache not enabled, pull from data mem 
		else{
			memStartFetch(mem, cpu->regs[x.trg], &cpu->regs[x.dest]);
			cpu->state = WAIT;
			cpu->pc++;
		}
	}
	
	//Store word instruction - 1 cycle or speed of mem
	else if(x.type == 0b110){
		//Cache enabled
		if(cache->enabled){
			
			cache->calculated = cpu->regs[x.trg]/8;
			unsigned int rem = cpu->regs[x.trg] % 8;
			
			bool cache_valid = false;
			for(int i = 0; i < 8; i++)
				if(cache->states[i] == 'V')
					cache_valid = true;

			//Write modified cache data to data mem
			if(cpu->regs[x.trg] == 0xFF){
				bool modified = false;
				for(int i = 0; i < 8; i++)
					if(cache->written[i])
						modified = true;
				if(modified){ mem->state = CACHE_WRITE; }
			}

			//Cache hit	
			else if(cache->clo == cache->calculated || !cache_valid){
				cache->data[rem] = cpu->regs[x.src];
				cache->written[rem] = true;
			}
			
			//Cache miss, cache flush
			else{
				bool cache_written = false;
				for(int i = 0; i < 8; i++)
					if(cache->written[i])
						cache_written = true;

				if(cache_written){
					for(int i = 0; i < 8; i++)
						cache->written[i] = false;
					cache->clo = cache->calculated;
					cache->data[rem] = cpu->regs[x.src];
					cache->written[rem] = true;
					mem->state = CACHE_WRITE;
				}
			}
			cpu->state = WAIT;
			cpu->pc++;
		}
		//Cache not enabled, pull from data mem 
		else{
			memStartStore(mem, cpu->regs[x.trg], &cpu->regs[x.src]);
			//mem.data[cpu->regs[x.trg]] = cpu->regs[x.src];
			cpu->state = WAIT;
			cpu->pc++;
		}
	}
	
	//Add instruction - 1 cycle
	else if(x.type == 0b000){
		cpu->regs[x.dest] = cpu->regs[x.src] + cpu->regs[x.trg];
		cpu->pc++;
	}
	
	//Addi instruction - 1 cycle
	else if(x.type == 0b001){
		cpu->regs[x.dest] = cpu->regs[x.src] + x.val;
		cpu->pc++;
	}

	//Mul instruction - 2 cycles
	else if(x.type == 0b010){
		cpu->regs[x.dest] = (cpu->regs[x.src] & 0b1111) * ((cpu->regs[x.src] >> 4) & 0b1111);
		cpu->pc++;
	}

	//Inv instruction - 1 cycle
	else if(x.type == 0b011){
		cpu->regs[x.dest] = ~cpu->regs[x.src] & 255;
		cpu->pc++;
	}
	
	//Branch instructions
	//TODO
	else if(x.type == 0b100){
		
		//Beq instruction - 1 cycle if regs equal, 2 if branch
		if(x.dest == 0b000){
			if(cpu->regs[x.src] == cpu->regs[x.trg]){
				cpu->state = WAIT;
				cpu->pc = x.val;
			}
			else
				cpu->pc++;
		}
		
		//Bneq instruction - 1 cycle if regs equal, 2 if branch
		else if(x.dest == 0b001){
			if(cpu->regs[x.src] != cpu->regs[x.trg]){
				cpu->state = WAIT;
				cpu->pc = x.val;
			}
			else
				cpu->pc++;
		}
		
		//Blt instruction - 1 cycle if src < trg, 2 if branch
		else if(x.dest == 0b010){
			if(cpu->regs[x.src] <= cpu->regs[x.trg]){
				cpu->state = WAIT;
				cpu->pc = x.val;
			}
			else
				cpu->pc++;
		}
	}
	
	//Halt instruction - 1 cycle
	else if(x.type == 0b111){
		cpu->halted = true;
		cpu->pc++;
	}

}

//Shifting registers, only for Assignment 1
void cpu_tick(CPU* cpu){
	for(int i = 7; i > 0; i--)
		cpu->regs[i] = cpu->regs[i-1];
}

void cpuStartTick(CPU* cpu, Memory* mem, Cache* cache){
	cpu->tc++;
	switch(cpu->state){
		case IDLE: cpu->state = FETCH; break;
		case FETCH: return;  //Get instruction from imem
		case DECODE: return; //Decode
		case MEMREQ: return; //LW or SW instruction
		case WAIT:
			if(mem->state == IDLE){
				cpu->state = IDLE;
			};
	}
}

void cpuDoCycleWork(CPU* cpu, Memory* mem, IMemory* imem, Cache* cache){
	if(!cpuIsMoreCycleWorkNeeded(cpu)) return;
	decode(cpu, mem, imem, cache);
}

bool cpuIsMoreCycleWorkNeeded(CPU* cpu){ return cpu->state != IDLE && cpu->state != WAIT && !cpu->halted; }
