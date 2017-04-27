#include "memory.h"

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

int allocateMemory(char* message){
  char *buf1, *buf2;
  buf1 = message;
  while (*buf1 != ' '){
    *buf2 = *buf1;
    buf1++;
    buf2++;
    *buf2 = '\0';
  }
  int memory = atoi(buf2);
  printf("%d", memory);
  return 0;
}
