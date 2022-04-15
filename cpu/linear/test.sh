#!/bin/bash

# 不间隔 CPU
test_pi()
{
	for ((i=4; i<$(nproc); i+=4))
	do
		max_cpu=`expr $i - 1`
		Usec=`build/linear -a $max_cpu -n 100 -t pi | grep Average | awk '{print $4}'`
		echo -e "$i \t $Usec"
	done
}

# 一亿次 计算
test_type()
{
	type=$1
	echo "========== $type ==========="
	for ((i=4; i<=$(nproc); i+=4))
	do
		Ncpu="`expr $i - 1`"
		max_cpu=`expr $i - 1`
		Usec=`build/linear -N 0 -a $max_cpu -t $type -n 1 |\
			grep Average | awk '{print $4}'`
		echo -e "$i \t $Usec"
	done
}

test_arithmetic()
{
	test_type arithmetic,integer,add
	test_type arithmetic,integer,sub
	test_type arithmetic,integer,mul
	test_type arithmetic,integer,div

	test_type arithmetic,float,add
	test_type arithmetic,float,sub
	test_type arithmetic,float,mul
	test_type arithmetic,float,div
}

test_arithmetic
