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
    if (frames[i].validBit && !frames[i].waiting){
      frames[i].validBit = false;
      return i;
    }
  }
  return -1;
}

void cleanMem(int table[MAXP][PAGES], frame_t frames[FRAMES], int process){

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

void printFrames(frame_t frames[FRAMES]){
  int i;
  for (i = 0; i < FRAMES; i++){
    if (frames[i].validBit && frames[i].process != -1)
      printf("U");
    else if(frames[i].validBit && !frames[i].waiting)
      printf("F");
    else printf("V");
  }
}

bool sweep(frame_t frames[FRAMES], int table[MAXP][PAGES]){
  int count = 0, i;
  for (i = 0; i < FRAMES; i++){
    if (!frames[i].validBit) count++;
  }
  if (count < (FRAMES * .9)) return false;

  int low = (int)(FRAMES * .05);
  int lowFrames[low];
  for (count = 0; count < low; count++){
    lowFrames[count] = count;
  }
  for (count = low; count < FRAMES; count++){
    for (i = 0; i < low; i++){
      if(timeIsUp(frames[i].timer, frames[count].timer)
          && !frames[count].waiting){
        lowFrames[i] = count;
      }
    }
  }
  for (count = 0; count < low; count++){
    if (!frames[count].validBit){
      frames[count].validBit = true;
    }
    else table[frames[count].process][frames[count].page] = -1;
  }
  return true;
}
