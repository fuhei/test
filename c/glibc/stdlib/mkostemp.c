#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int mkstemp (char *template);


int main()
{
					 ///home/rongtao/test/c/glibc/stdlib
	//char file[256] = {"/home/rongtao/test/c/glibc/stdlib/"};;
	char file[256] = {"/tmp/XXXX.out"};

	printf("file: %s\n", file);
	int ret = mkostemp(file, O_RDWR|O_CREAT);

	perror("mkstemp: ");
	printf("file: %s, ret = %d\n", file, ret);



	return 0;
}
