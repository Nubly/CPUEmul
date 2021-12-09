#include <stdio.h>
#include <stdlib.h>
#include "imemory.h"

void imem_create(IMemory* imem, int size){
	imem->size = size;
	//Sizeof unsigned int is 4 bytes, certainly enough for 20 bit numbers
	//Technically wasting some memory here, but oh well
	imem->data = (unsigned int*) malloc(sizeof(unsigned int) * size);
}

void imem_reset(IMemory* imem){
	for(unsigned int i = 0; i < imem->size; i++)
		imem->data[i] = 0;
}

void imem_dump(IMemory* imem, int addr, int count){
	printf("Addr     0     1     2     3     4     5     6     7\n");
	for(int i = 0; i < addr+count && i < imem->size; i++){
		//Beginning characters
		if(i % 8 == 0) printf("0x%02X ", i);
		//Empty spaces for bytes not in range
		if(i < addr) printf("      ");
		else printf("%05X", imem->data[i]);
		//Ending characters
		if((i+1) % 8 == 0) printf("\n");
		else printf(" ");
	}
	printf("\n");
}

void imem_set(IMemory* imem, int addr, FILE* file){
	int i = 0;
	while(fscanf(file, "%5X", &imem->data[i+addr]) == 1 && i+addr <= imem->size) i++;
}
