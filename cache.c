#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cache.h"

void cache_reset(Cache* cache){
	cache->clo = 0;
	cache->enabled = false;
	for(int i = 0; i < 8; i++)
		cache->states[i] = 'I';
}

void cache_dump(Cache* cache){
	printf("CLO        : 0x%02X\n", cache->clo);
	printf("cache data : ");
	for(int i = 0; i < 8; i++)
		printf("0x%02X ", cache->data[i]);
	printf("\nFlags      : ");
	for(int i = 0; i < 8; i++){
		if(cache->written[i])
			printf("  %c  ", 'W');
		else
			printf("  %c  ", cache->states[i]);
	}
	printf("\n\n");
}

//void cache_write(Memory* mem, Cache* cache, int addr){
//	bool modified = false;
//	for(int i = 0; i < 8; i++)
//		if(cache->states[i] == 'W')
//			modified = true;
//	if(!modified){ return; }
//	if(mem->ctr == 5){
//		for(int i = 0; i < 8; i++)
//			if(cache->states[i] == 'W')
//				mem->data[addr+(sizeof(int)*i)] = cache->data[i];
//		mem->state = IDLE;
//		mem->ctr = 0;
//	}	
//}
