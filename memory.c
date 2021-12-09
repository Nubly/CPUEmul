#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "memory.h"
#include "cache.h"

void mem_create(Memory* mem, int size){
	mem->size = size;
	mem->data = (unsigned int*) malloc(sizeof(int)*size);
}

void mem_reset(Memory* mem){
	for(int i = 0; i < mem->size; i++)
		mem->data[i] = 0;
}

void mem_dump(Memory* mem, int addr, int count){
	printf("Addr 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	for(int i = 0; i < addr+count && i < mem->size; i++){
		//Beginning characters
		if(i % 16 == 0)	printf("0x%02X ", i);
		//Empty spaces for bytes not in range
		if(i < addr) printf("   ");
		else printf("%02X", mem->data[i]);
		//Ending characters
		if((i+1) % 16 == 0 || i == addr+count - 1) printf("\n");
		//Print regularly
		else printf(" ");
	}
	printf("\n");
}

void mem_set(Memory* mem, int addr, int count, int* data){
	for(int i = 0; i < count; i++){
		mem->data[addr+i] = data[i];
	}
}

void memStartTick(Memory* mem){ mem->ctr++; }

void memDoCycleWork(Memory* mem, Cache* cache){
	//Finished waiting, can move mem
	if(mem->ctr == 5){
		if(mem->state == READ)	
			*mem->trgPtr = mem->data[mem->addr];
		else if(mem->state == WRITE)
			mem->data[mem->addr] = *mem->trgPtr;
		else if(mem->state == CACHE_READ)
			for(int i = 0; i < 8; i++){
				cache->data[i] = mem->data[(8*cache->calculated)+i];
				cache->states[i] = 'V';
			}
		else if(mem->state == CACHE_WRITE)
			for(int i = 0; i < 8; i++)
				if(cache->written[i])
					cache->data[i] = mem->data[(8*cache->calculated)+i];	
		mem->state = IDLE;
		mem->ctr = 0;
	}
}

bool memIsMoreCycleWorkNeeded(Memory* mem){ 
	//return mem->state != IDLE && mem->state != WAIT;
	return false;
}

void memStartFetch(Memory* mem, unsigned int addr, unsigned int* trgPtr){
	mem->addr = addr;
	mem->trgPtr = trgPtr;
	mem->state = READ;
	mem->ctr = 0;
}

void memStartStore(Memory* mem, unsigned int addr, unsigned int* trgPtr){
	mem->addr = addr;
	mem->trgPtr = trgPtr;
	mem->state = WRITE;
	mem->ctr = 0;
}

