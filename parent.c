#include "system.h"

void initialFork();
void masterHandler(int signum);
bool requestMgmt(mymsg_t*);
void cleanUp(int);

int x = 4;			//starting processes

struct sigaction act;
int queueid;
system_t* sysid;

FILE* fptr;

int main(int argc, char **argv){
	fprintf(stderr, "Master pid: %d\n", getpid());
	mymsg_t message;
	int i;
	frame_t memory[FRAMES];
	int table[MAXP][PAGES];
	initMem(memory, table);
	//fptr = fopen("a.out", "w");

/*	while ((i = getopt (argc, argv, "v")) != -1){
	switch (i){
	case 'v': verbose = true; break;
	default: fprintf(stderr, "Invalid command line argument\n");
		exit(-1);
	}}
*/
	act.sa_handler = masterHandler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGALRM, &act, 0);
	sigaction(SIGINT, &act, 0);
	sigaction(SIGTERM, &act, 0);
	alarm(10);			//failsafe

	queueid = initqueue();		//initializes and attaches msgqueue
	sysid = getSystem();		//initializes and attaches clock and child array

	initClock(sysid);

	//spawns originial processes in system
	for (i = 0; i < x; i++){
		initialFork();
	}

	//loop runs until system clock passes 2 seconds
	while (updateClock(1000000, sysid)){
		if (msgrcv(queueid, &message, MSGSIZE, -3, IPC_NOWAIT) == MSGSIZE){
			int mem = parseMessage(&message);
			int pid = 0;
			while (message.mtype != sysid->children[pid]){
				pid++;
			}
			if (table[pid][mem] != -1){
				printClock(sysid);
				printf("Page already loaded in %d\n", table[pid][mem]);
				memory[table[pid][mem]].dirtyBit = true;
				msgsnd(queueid, &message, MSGSIZE, 0);
			}
			else {
				table[pid][mem] = assignMemory(memory);
				if (table[pid][mem] == -1){
					printf("\tPage Fault\n");
				}
				else{
					printClock(sysid);
					printf("%d %d loaded in %d\n", sysid->children[pid], mem, table[pid][mem]);
					msgsnd(queueid, &message, MSGSIZE, 0);
				}
			}
		}

		//checks if it's time to spawn another user process
		if (timeIsUp(sysid)){
			initialFork();
			setTimer(sysid);		//sets new timer for next user process
		}
	}
	masterHandler(0);
}

//forks a child in the lowest slot available in child array
void initialFork(){
	int i = 0;
	while (sysid->children[i] != 0){
		i++;
		if (i == MAXP) return;
	}
	pid_t childPid;
	switch (childPid = fork()){
        	case -1: perror("problem with fork()ing"); break;
        	case 0: execl("userProcess", "./userProcess", NULL); break;
        	default: sysid->children[i] = childPid;
	}
}

//kills any children and waits for their response to continue
void cleanUp(int i){
	if (sysid->children[i] > 0){
		kill(sysid->children[i], SIGINT);
		waitpid(sysid->children[i],0,0);
		printClock(sysid);
		printf("%d has termed\n", sysid->children[i]);
		sysid->children[i] = 0;
	}
}

//releases all shared memory and cleans up remaining children.
void masterHandler(int sig){
	int i;
	for (i = 0; i < MAXP; i++){
		cleanUp(i);
	}
	msgctl(queueid, IPC_RMID,0 );
	releaseClock(&sysid, 'd');
	//fclose(fptr);
	exit(1);
}
