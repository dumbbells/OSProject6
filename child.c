#include "system.h"

void childHandler(int sig);		//function to detach from all shared mem
void initChild();							//function to set up initial values of child
void setReqTimer();						//sets timer until next request
bool timeUp();							//checks to see if it should term or continue
bool reqTime();
void getPage();

struct sigaction act;
int queueid, quantum = 80000;
system_t childData;						//used this struct twice to hold timers
system_t* sysid;
mymsg_t message;

int main(int argc, char **argv){
	int i;

	srand(getpid());

	act.sa_handler = childHandler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGALRM, &act, NULL);
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGTERM, &act, NULL);
	alarm(4);				//fail safe

	//attaches to shared memory
	queueid = initqueue();
	sysid = getSystem();

	//initializes data for the child including when to request or release
	initChild(quantum);
	setReqTimer(quantum);

	//loop runs until the system clock is greater than 1 second
	while(updateClock(quantum, sysid) && !(timeUp())){
		if (reqTime()){
			getPage();
			setReqTimer(quantum);	//resets timer for next round of requests
		}
	}
	printf("im dead lol\n");
	releaseClock(&sysid, ' ');
	exit(1);
}

void getPage(){
	message.mtype = 2;
	int page = (rand() % PAGES);
	sprintf(message.mtext, "%02d %06d", page, getpid());
	//printClock(sysid);
	//printf("%d: %s\n", getpid(), message.mtext);
	msgsnd(queueid, &message, MSGSIZE, 0);
	msgrcv(queueid, &message, MSGSIZE, getpid(), 0);
	//printf("msg rec\n");
}

//determines if process should terminate on it's own terms. Most likely
//will continue on. message type 1 (highest priority) if it terms
bool timeUp(){
	if (sysid->clock[0] > childData.timer[0] || (
			sysid->clock[0] == childData.timer[0] &&
			sysid->clock[1] >= childData.timer[1])){
		if (rand()%4 == 0){
			sprintf(message.mtext, "00 %d", getpid());
			message.mtype = 1;
			msgsnd(queueid, &message, MSGSIZE, 0);
			return true;
		}
		else{
			initChild();		//otherwise set new term time
		}
	}
	return false;
}

bool reqTime(){
	if (sysid->clock[0] > childData.clock[0] || (
		sysid->clock[0] == childData.clock[0] &&
		sysid->clock[1] >= childData.clock[1])){
	return true;
}
return false;
}

//uses timer from system.h
void initChild(){
	message.mtype = getpid();
	childData.timer[1] = sysid->clock[1] + (rand()%quantum)*1000;
	rollOver(childData.timer);		//in system.c, carries 1 if necessary
}

//uses clock from system.h
void setReqTimer(){
	childData.clock[1] = sysid->clock[1] + (rand()%quantum)*10;
	rollOver(childData.clock);
}

void childHandler(int sig){
	releaseClock(&sysid, 'd');
	exit(1);
}
