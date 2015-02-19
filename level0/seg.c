#include <stdio.h>
#define BUF_SIZE 10
void readBuf(){
  char buf[BUF_SIZE];
  gets(buf);
}
int main(){
  readBuf();
  return 0;
}
