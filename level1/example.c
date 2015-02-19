#include <stdio.h>
#define BUF_SIZE 8

int readBuf(){
  int a = 0;
  char buf[BUF_SIZE];
  gets(buf);
  return a;
}

void test(){
  int ret = readBuf();
  if(ret == 0)
    printf("Value is normal :(\n");
  else
    printf("You modified the value into: %d\n", ret);
}

int main(){
  test();
  return 0;
}
