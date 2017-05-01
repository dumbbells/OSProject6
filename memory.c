#include "memory.h"
#include "system.h"
int allocateMemory(mymsg_t *message);

void initMem(frame_t frames[FRAMES], int table[MAXP][PAGES]){
  for (int i = 0; i < FRAMES; i++){
    frames[i].dirtyBit = false;
    frames[i].waiting = false;
  }
  for (int i = 0; i < MAXP; i++){
    for (int k = 0; k < PAGES; k++){
      table[i][k] = -1;
    }

  }
  printf("Memory initialized\n");
}

int allocateMemory(mymsg_t* message){
  printf("%s\n", message->mtext);
  int count = 0;
  char temp[6];
  while (count < 2){
    temp[count] = message->mtext[count];
    count++;
    temp[count] = '\0';
  }
  count++;
  int memory = atoi(temp);
  while (message->mtext[count] != '\0'){
      temp[count - 3] = message->mtext[count];
      count++;
      temp[count - 3] = '\0';
  }
  int pid = atoi(temp);
  message->mtype = pid;

  printf("%d requested by %d\n", memory, pid);
  return memory;
}
