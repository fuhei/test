#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

int _trampoline(void)
{
	return printf("--- ftrace ---\n");
}

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
