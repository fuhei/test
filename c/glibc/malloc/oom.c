#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <mcheck.h>

int main()
{
	int i;
	char *a = NULL;
	
	while(1) {
		a = malloc(40960);
		for (i = 0; i < 40960; i+=4096) {
			a[i] = 'a';
		}
	}
	return 0;
}
