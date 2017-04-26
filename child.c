#include "includes.h"

void childHandler(int sig);		//function to detach from all shared mem
void initChild();							//function to set up initial values of child
void setReqTimer();						//sets timer until next request
void timeUp();							//checks to see if it should term or continue

struct sigaction act;
int queueid, quantum = 800000;
system_t childData;						//used this struct twice to hold timers
system_t* sysid = NULL;
memCtrl* rscid = NULL;
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
	while(updateClock(quantum, sysid)){
		timeUp();							//checks to see if it's time to term
			setReqTimer(quantum);	//resets timer for next round of requests
	}
	childHandler(0);		//detaches from all shared mem
}

//determines if process should terminate on it's own terms. Most likely
//will continue on. message type 1 (highest priority) if it terms
void timeUp(){
	if (sysid->clock[0] > childData.timer[0] || (
			sysid->clock[0] == childData.timer[0] &&
			sysid->clock[1] >= childData.timer[1])){
		if (rand()%4 == 0){
			sprintf(message.mtext, "00 %d", getpid());
			message.mtype = 1;
			msgsnd(queueid, &message, MSGSIZE, 0);
			childHandler(0);
		}
		else{
			initChild();		//otherwise set new term time
		}
	}
}

//uses timer from system.h
void initChild(){
	childData.timer[1] = sysid->clock[1] + (rand()%quantum)*1000;
	rollOver(&childData);		//in system.c, carries 1 if necessary
}

//uses clock from system.h
void setReqTimer(){
	childData.clock[1] = sysid->clock[1] + (rand()%quantum)*10;
	rollOver(&childData);
}

void childHandler(int sig){
	if(sysid != NULL) releaseClock(&sysid, ' ');
	exit(1);
}
