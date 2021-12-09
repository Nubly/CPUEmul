#include <stdbool.h>
#ifndef CACHE
#define CACHE

typedef struct {
	unsigned int clo, calculated;
	unsigned int data[8];
	unsigned char states[8];
	bool written[8];
	bool enabled;
} Cache;

void cache_reset(Cache* cache);

void cache_dump(Cache* cache);

#endif
