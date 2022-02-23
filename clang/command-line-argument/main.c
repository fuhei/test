#include <stdio.h>

int main()
{
#ifdef __clang__
	printf("clang:\n");
#endif
}
