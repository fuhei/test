#include <stdio.h>

int a = 100;

void _trampoline(void)
{
	printf("-TRAMPOLINE- %d -", a);
}

