#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "clock.h"
#include "CPU.h"
#include "memory.h"
#include "imemory.h"
#include "cache.h"

//Max length of command is 7 utf-8 chars
char cmd[8];

Clock clk;
CPU cpu;
Memory mem;
IMemory imem;
Cache cache;

int main(int argc, char* argv[]){

	if(argc < 2){
		printf("Usage: cs3421_emul <input filename>\n");
		return 0;
	}

	FILE* input = fopen(argv[1], "r");

	if(input == NULL) {
		printf("Failed to open input file, ensure it is present and readable.\n");
		return -1;
	}

	//Initialize mem counter to 0
	mem.ctr = 0;

	while(fscanf(input, "%s", cmd) == 1){

		//Check clock
		if(strcmp(cmd, "clock") == 0){
			fscanf(input, "%s", cmd);
			if(strcmp(cmd, "reset") == 0){ clk_reset(&clk); continue; }
			else if(strcmp(cmd, "tick") == 0){
				int ticks;
				fscanf(input, "%d", &ticks);
				tick(&clk, &mem, &imem, &cpu, &cache, ticks);
			}
			else if(strcmp(cmd, "dump") == 0){ clk_dump(&clk); continue; }
		}	

		//Check memory
		else if(strcmp(cmd, "memory") == 0){
			fscanf(input, "%s", cmd);
			if(strcmp(cmd, "create") == 0){
				int size;
				fscanf(input, "%x", &size);
				mem_create(&mem, size);
				continue;
			}
			else if(strcmp(cmd, "reset") == 0){ mem_reset(&mem); continue; }
			else if(strcmp(cmd, "dump") == 0){
				int addr, count;
				fscanf(input, "%x %x", &addr, &count);
				mem_dump(&mem, addr, count);
				continue;
			}
			else if(strcmp(cmd, "set") == 0){
				int memaddr, count;
				fscanf(input, "%4x %4x", &memaddr, &count);
				int data[count];
				for(int i = 0; i < count; i++){
					fscanf(input, "%4x", &data[i]);
				}
				mem_set(&mem, memaddr, count, data); 
				continue;
			}
		}

		//Check cpu
		else if(strcmp(cmd, "cpu") == 0){
			fscanf(input, "%s", cmd);
			if(strcmp(cmd, "reset") == 0){ cpu_reset(&cpu); continue; }
			else if(strcmp(cmd, "set") == 0){
				char regName[2];
				int value;
				fscanf(input, "%*s %s %x", regName, &value);
				cpu_set(&cpu, regName, value);
				continue;
			}
			else if(strcmp(cmd, "dump") == 0){ cpu_dump(&cpu); continue; }
		}

		//Check imemory
		else if(strcmp(cmd, "imemory") == 0){
			fscanf(input, "%s", cmd);
			if(strcmp(cmd, "create") == 0){
				int size;
				fscanf(input, "%x", &size);
				imem_create(&imem, size);
				continue;
			}
			else if(strcmp(cmd, "reset") == 0){ imem_reset(&imem); continue; }
			else if(strcmp(cmd, "dump") == 0){
				int addr, count;
				fscanf(input, "%x %x", &addr, &count);
				imem_dump(&imem, addr, count);
				continue;
			}
			else if(strcmp(cmd, "set") == 0){
				int memaddr;
				char filename[100];
				fscanf(input, "%x %*s %s", &memaddr, filename);
				FILE* imem_inst = fopen(filename, "r");
				if(imem_inst == NULL) { 
					printf("Failed to open imem instruction file.\n");
					return -1;
				}
				imem_set(&imem, memaddr, imem_inst); 
				continue;
			}
		}
		//Check cache
		else if(strcmp(cmd, "cache") == 0){
			fscanf(input, "%s", cmd);
			if(strcmp(cmd, "reset") == 0){ cache_reset(&cache); }
			if(strcmp(cmd, "on") == 0){ cache.enabled = true; }
			if(strcmp(cmd, "off") == 0){ cache.enabled = false; }
			if(strcmp(cmd, "dump") == 0){ cache_dump(&cache); }
		}
	}
	free(input);
	return 0;
}

//int parse(FILE* input){
//	char* cmd = fscanf(input, "%s", cmd);
//	if
