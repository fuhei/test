#include <malloc.h>
#include <unistd.h>

int main()
{
	int i;
	unsigned long int size = getpagesize()*1000;
	char *str = malloc(size);

	for (i=0; i<size; i+=getpagesize()) {
		str[i] = 'a';
	}

	free(str);
}
