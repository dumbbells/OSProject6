#include "system.h"

void initialFork();
void masterHandler(int signum);
bool requestMgmt(mymsg_t*);
void cleanUp(int);

int x = 4;			//starting processes

struct sigaction act;
int queueid;
system_t* sysid;
int table[MAXP][PAGES];

FILE* fptr;

int main(int argc, char **argv){
	fprintf(stderr, "Master pid: %d\n", getpid());
	mymsg_t message;
	int i;
	frame_t memory[FRAMES];
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
	setTimer(sysid->clock, sysid->timer);

	//spawns originial processes in system
	for (i = 0; i < x; i++){
		initialFork();
	}

	//loop runs until system clock passes 2 seconds
	while (updateClock(1000, sysid)){
		for (i = 0; i < FRAMES; i++){
			if (memory[i].waiting && timeIsUp(sysid->clock, memory[i].timer)){
				message.mtype = sysid->children[memory[i].process];
				memory[i].waiting = false;
				msgsnd(queueid, &message, MSGSIZE, 0);
			}
		}
		if (msgrcv(queueid, &message, MSGSIZE, -3, IPC_NOWAIT) == MSGSIZE){
			int pid = 0;
			if (message.mtype == 1){
				parseMessage(&message);
				while (message.mtype != sysid->children[pid]){
					pid++;
				}
				cleanUp(pid);
				continue;
			}
			int mem = parseMessage(&message);
			while (message.mtype != sysid->children[pid]){
				pid++;
			}
			if (table[pid][mem] != -1){
				updateClock(10, sysid);
				printClock(sysid);
				printf("Page already loaded continuing %ld\n", message.mtype);
				msgsnd(queueid, &message, MSGSIZE, 0);
			}
			else {
				table[pid][mem] = assignMemory(memory);
				if (table[pid][mem] == -1){
					printf("\tPage Fault\n");
				}
				else{
					memory[table[pid][mem]].process = pid;
					memory[table[pid][mem]].page = mem;
					memory[table[pid][mem]].waiting = true;
					memory[table[pid][mem]].validBit = false;
					memory[table[pid][mem]].timer[1] = sysid->clock[1] + (1.5 * pow(10,7));
					rollOver(memory[table[pid][mem]].timer);

					printClock(sysid);
					printf("%d %d loaded in %d\n", sysid->children[pid], mem, table[pid][mem]);
					//msgsnd(queueid, &message, MSGSIZE, 0);
				}
			}
			//message.mtype = 4;
		}

		//checks if it's time to spawn another user process
		if (timeIsUp(sysid->clock, sysid->timer)){
			initialFork();
			setTimer(sysid->clock, sysid->timer);		//sets new timer for next user process
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
	int k;
	if (sysid->children[i] > 0){
		kill(sysid->children[i], SIGINT);
		waitpid(sysid->children[i],0,0);
		printClock(sysid);
		printf("%d has termed\n", sysid->children[i]);
		for (k = 0; k < PAGES; k++){
			table[i][k] = -1;
		}
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
