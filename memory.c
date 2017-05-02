#include "memory.h"
#include "system.h"
int allocateMemory(mymsg_t *message);
int assignMemory(frame_t frames[FRAMES]);

void initMem(frame_t frames[FRAMES], int table[MAXP][PAGES]){
  for (int i = 0; i < FRAMES; i++){
    frames[i].dirtyBit = false;
    frames[i].waiting = false;
    frames[i].validBit = true;
  }
  for (int i = 0; i < MAXP; i++){
    for (int k = 0; k < PAGES; k++){
      table[i][k] = -1;
    }

  }
  printf("Memory initialized\n");
}

int assignMemory(frame_t frames[FRAMES]){
  int i;
  for (i = 0; i < FRAMES; i++){
    if (frames[i].validBit){
      frames[i].validBit = false;
      printf("loading frame %d\n", i);
      return i;
    }
  }
  return -1;
}

int parseMessage(mymsg_t* message){
  int count = 0;
  char temp[6];
  while (count < 2){
    temp[count] = message->mtext[count];
    count++;
    temp[count] = '\0';
  }
  count++;
  int memory = atoi(temp);
  while (count < MSGSIZE){
      temp[count - 3] = message->mtext[count];
      count++;
      temp[count - 3] = '\0';
  }
  int pid = atoi(temp);
  message->mtype = pid;
  return memory;
}
