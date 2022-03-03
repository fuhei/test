#include <stdio.h>
#include <unistd.h>

int func2(void)
{
	static unsigned long i = 0;
	printf("--- Child %d ---\n", i++);
	return 'R';
}

int func1(void)
{
	return func2();
}

int main(int argc, char *argv[])
{
	while (1) {
		func1();
		sleep(2);
	}
}
