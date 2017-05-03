#include "system.h"

void initMem(frame_t frames[FRAMES], int table[MAXP][PAGES]){
  for (int i = 0; i < FRAMES; i++){
    frames[i].process = -1;
    frames[i].page = -1;
    frames[i].waiting = false;
    frames[i].validBit = true;
    frames[i].timer[0] = 0;
    frames[i].timer[1] = 0;
  }
  for (int i = 0; i < MAXP; i++){
    for (int k = 0; k < PAGES; k++){
      table[i][k] = -1;
    }
  }
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
