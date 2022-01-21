
sort:
	gcc sort.c -o sort.out
sort_optimized:
	gcc -O3 sort.c -o sort_optimized-O3.out
sort_fdo:
	gcc sort.c -o sort_instrumented.out -fprofile-generate
sort_fdo_again:
	gcc -O3 sort.c -o sort_fdo.out -fprofile-use=sort.gcda
clean:
	rm -f *.out *.gcda

