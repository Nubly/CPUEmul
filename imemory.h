#ifndef IMEMORY
#define IMEMORY

#include <stdio.h>

typedef struct {
	unsigned int size;
	unsigned int* data;
} IMemory;

void imem_create(IMemory* imem, int size);

void imem_reset(IMemory* imem);

void imem_dump(IMemory* imem, int addr, int count);

void imem_set(IMemory* imem, int addr, FILE* file);

#endif
