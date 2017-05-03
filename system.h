/*
 * system.h
 *
 *  Created on: Apr 6, 2017
 *      Author: gregclimer
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_
#define MAXP 18
#define MSGSIZE 9
#define PAGES 32
#define FRAMES 256

#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <math.h>
#include <sys/stat.h>
#include <stdbool.h>


//used to track child timers as well as system clock
//children is an array of live child pids
typedef struct{
	unsigned long clock[2];
	unsigned long timer[2];
	pid_t children[MAXP];
} system_t;

//declaration of message struct
typedef struct{
	long mtype;
	char mtext[MSGSIZE];
} mymsg_t;

typedef struct{
	int process;
	int page;
	bool waiting;
	bool validBit;
	unsigned long timer[2];
} frame_t;

void initMem(frame_t[FRAMES], int[MAXP][PAGES]);
int parseMessage(mymsg_t *message);
int assignMemory(frame_t frames[FRAMES]);
bool sweep(frame_t frames[FRAMES], int table[MAXP][PAGES]);
void printFrames(frame_t frames[FRAMES]);

void errorCheck (int i, char* string);

void printClock(system_t*);
bool timeIsUp(unsigned long clock[2], unsigned long timer[2]);
int initqueue();
system_t* getSystem();
void releaseClock(system_t** ptr, char name);
void initClock(system_t* clock);
bool updateClock(int increment, system_t* clock);
void rollOver(unsigned long clock[2]);
void setTimer(unsigned long clock[2], unsigned long timer[2]);

#endif /* SYSTEM_H_ */
