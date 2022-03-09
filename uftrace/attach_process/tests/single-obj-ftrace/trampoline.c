#include <stdio.h>

int _trampoline(void)
{
	int n = printf("\033[1;31m-I AM TRAMPOLINE-\033[m");
	fflush(stdout);
	return n;
}

extern void my_tramp(void);

asm (
"   .pushsection    .text, \"ax\", @progbits\n"
"   .type       my_tramp, @function\n"
"   .globl      my_tramp\n"
"   my_tramp:"
"   pushq %rbp\n"
"   movq %rsp, %rbp\n"
"   pushq %rdi\n"
"   call _trampoline\n"
"   popq %rdi\n"
"   leave\n"
"   ret\n"
"   .size       my_tramp, .-my_tramp\n"
"   .popsection\n"
);

int func3()
{
	_trampoline();
	return printf("func3.\n");
}