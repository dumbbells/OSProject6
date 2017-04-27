#ifndef MEMORY_H_
#define MEMORY_H_

#include "system.h"
#include <stdbool.h>

//int shmRsc;

typedef struct{
	bool dirtyBit;
	bool waiting;
} frame_t;

void initMem(frame_t[FRAMES], int[MAXP][PAGES]);
int allocateMemory(char*);

#endif
