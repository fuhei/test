#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

int func2(void)
{
	static unsigned long i = 0;
	printf("--- Child %d ---\n", i++);
	unsigned long size = 8192*3;
	void *a = mmap(0UL, size,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	munmap(a, size);
	return 'R';
}

int func1(void)
{
	return func2();
}

int main(int argc, char *argv[])
{
	sleep(2);
	printf("--- Child mmap ---\n");
	sleep(2);
	while (1) {
		func1();
		sleep(2);
	}
}
