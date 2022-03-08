#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

int _trampoline(void)
{
	return printf("--- ftrace ---\n");
}

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

int func2(void)
{
	return printf("func2: %p, (func3: %p), (_trampoline: %p)\n", func2, func3, _trampoline);
}

int func1(void)
{
	printf("func1: %p, ", func1);
	return func2();
}

int main(int argc, char *argv[])
{
	sleep(2);
	printf("--- Child mmap ---\n");
	sleep(2);
	while (1) {
		func1();
		sleep(1);
	}
}
