#include "multi-srclib.h"

int main(int argc, char *argv[])
{
	int i;
	for (i=0; i<1000000000; i++)
		do_nothing();
	return 0;
}
