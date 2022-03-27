#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

int func2(void)
{
	return printf("\033[1;32mfunc2: %p\033[m\n", func2);
}

int func1(void)
{
	printf("\033[1;33mfunc1: %p, \033[m", func1);
	return func2();
}

int main(int argc, char *argv[])
{
	sleep(2);
	printf("--- Child mmap ---\n");
	printf("func2  = %p, %ld\n", func2, (unsigned long )func2);
	printf("printf = %p, %ld\n", printf, (unsigned long )printf);
	sleep(2);
	while (1) {
		func1();
		sleep(1);
	}
}
