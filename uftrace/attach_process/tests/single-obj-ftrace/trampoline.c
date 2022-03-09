#include <stdio.h>

int _trampoline(void)
{
	int n = printf("\033[1;31m-I AM TRAMPOLINE-\033[m");
	fflush(stdout);
	return n;
}

extern void ftrace_tramp(void);

asm (
"   .pushsection    .text, \"ax\", @progbits\n"
"   .type       ftrace_tramp, @function\n"
"   .globl      ftrace_tramp\n"
"   ftrace_tramp:"
"   pushq %rbp\n"
"   movq %rsp, %rbp\n"
"   pushq %rdi\n"
"   call _trampoline\n"
"   popq %rdi\n"
"   leave\n"
"   ret\n"
"   .size       ftrace_tramp, .-ftrace_tramp\n"
"   .popsection\n"
);

int ___func3()
{
	_trampoline();
	return printf("func3.\n");
}
