#include <stdio.h>

void ftrace_routinue(void)
{
    printf("..mytramp..\n");
}

extern void trampoline(void *);

asm (
"       .pushsection    .text, \"ax\", @progbits\n"
"       .type       trampoline, @function\n"
"       .globl      trampoline\n"
"   trampoline:"
"       pushq %rbp\n"
"       movq %rsp, %rbp\n"
"       call ftrace_routinue\n"
"       leave\n"
"       ret\n"
"       .size       trampoline, .-trampoline\n"
"       .popsection\n"
);