#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("Hello1.\n");
	printf("Hello2.\n");
	if (argc > 1)
		goto label_0;
	printf("Hello3.\n");
	printf("Hello4.\n");
label_0:
	return 0;
}
