#include <stdio.h>
#define BUF_SIZE 8

void bomb(){
  printf("BANG!!!!!\n");
}

void readBuf(){
  char buf[BUF_SIZE];
  gets(buf);
}

int main(){
  readBuf();
  printf("exit normally :(\n");
  return 0;
}
