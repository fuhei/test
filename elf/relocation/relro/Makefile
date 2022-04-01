

all:
	gcc -g -Wl,-z,relro -o test.out testcase.c
	gcc -g -Wl,-z,relro,-z,now -o test.full.out testcase.c

clean:
	rm -f *.out *.o
