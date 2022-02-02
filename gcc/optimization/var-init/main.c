#include <stdlib.h>

int main(int argc, char *argv[])
{
	int op;
	if (argc > 1) {
		op = atoi(argv[1]);
	}

	asm volatile("movl $2, %eax");

	if (op == 1) {
		return 1;
	} else if (op == 2) {
		return 2;
	}

	return 0;
}
