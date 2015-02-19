#include <stdio.h>

#define BUF_SIZE 2000

int a = 0;

int readBuf(){
  char buf[BUF_SIZE];
  gets(buf);
  printf("The address of buffer: %x\n", buf);
  return 0;
}

void test(){
  readBuf();
  if(a != 1)
    printf("Return normally :(\n");
  else
    printf("haha, you hacked it! :)\n");
}

int main(){
  test();

  return 0;
}
