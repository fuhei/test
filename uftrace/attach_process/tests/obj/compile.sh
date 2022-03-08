

gcc -c f1.c -o f1.o
gcc -c f2.c -o f2.o
gcc -c f3-trampoline.c -o f3-trampoline.o
gcc -c main.c -o main.o

gcc f1.o f2.o f3-trampoline.o main.o
