# StackOverflow Level 0

> In this series of tutorials, you will be introduced to four levels of hacking challenges relating to the stack overflow problem in modern computer system. The goals are given before the real tutorial in case you'd like to do it yourself. After completing all challenges, you will have a deeper understanding of x86 ISA, Linux system, and binary program, and your C skill, x86 assembling skill, and debugging skill will be enhanced. What's more, you will have a more clear understanding of the security threat underlying in current computing world. Happy Hacking :)

## Goals
* Learn some basic concepts
* Make preparations for experiments
* Trigger your first `segment fault` intentionally

## Basic Concepts
To conduct hands-on experiments well, we needs to introduce some basic knowledge first. Even you are experienced in that field, it is still recommended to skim through this section :)

### The mysterious `ELF` format
`ELF` (Executable and Linkable Format) is the binary program format in the Linux world. A usual program binary is composed of three parts:

* `text`: The program itself
* `data`: The data area contains global and static variables used by the program that are explicitly initialized with a non-zero (or non-NULL) value.
* `bss`: BSS segment contains all global variables and static variables that are initialized to zero or do not have explicit initialization in source code.

### Stack Frame -- How is function calling implemented?
Every time we talked about this topic, we need to bring this classical illustration picture to the front:

![](stackframe.png)

What's more, we need to dive into the machine-level instructions as well as the two registers `rbp` and `rsp`.

When calling a subroutine in x86, we will simply pushed the arguments on the stack and `callq [address of subroutine]`. The `call` will implicitly push the current `%eip`, or the "return address" on the stack. Then, the control is handed over to the subroutine, which will first:

    push %rbp
    mov  %rsp, %rbp

, simply push the caller's frame base pointer on the stack so we can restore it back after subroutine. Then, we will use `%rsp` to extract the arguments from stack.

After the subroutine is finished, we will

    leave
    ret

The `leave` instruction is actually the synonym of

    mov %rsp, %rbp
    pop %rbp

And `ret` will set the `PC` to return address on the stack.

### Overflow -- Stack Smashing Attack
The notorious function `gets` will easily incur stack overflow problem. The local variable will be pushed onto stack. So, please imagine an array of `char`, if `gets()` doesn't check the length of input, the other data in the stack will be corrupted.

Especially, if return address is damaged, we may return to some mysterious place, which triggers `segmentation fault`. But, what if, we feed into some meaningful string which are intricately handcrafted?

## Preparations
Before hands-on operations, you needs a good experimental platform. Linux is recommended here. And it is recommended to conduct all experiment in a virtual machine.

Author is using:

* Ubuntu 14.04.1, GNU/Linux 3.13.0-45-generic x86_64
* GCC 4.8.2

Besides softwares configuration, You also needs to learn some basic usage of `gdb`, `as` and `objdump`. Such as:

1. Using `gdb` to set breakpoint, disassemble, step, check memory and registers value etc.
2. Using `as` to assemble hand-written x86 assembling code.
3. Using `objdump -d` to disassemble a binary program

So, as a result, the author assumed that you have already know some basics of C programming, x86 assembling language, basic debugging skills, and some primitive knowledge of computer system.

## `segment fault`, seriously
First, let's compile the first sample program with `gcc seg.c -o seg`, and neglect the warning about `gets` because we intentionally use this buggy function.
    
    // seg.c
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

here, when run it, we feed it into a string longer than 10, like `aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa`. And do you have a `segment fault` or something alike here?