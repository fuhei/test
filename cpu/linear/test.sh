#!/bin/bash

# 不间隔 CPU
test_1()
{
	for ((i=0; i<$(nproc); i++))
	do
		build/linear -a $i -n 100 -t pi | grep Average | awk '{print $3}'
	done
}

test_2_interval()
{
	for ((i=2; i<=8; i+=2))
	do
		nthread=`expr $(nproc) / $i`
		echo -e "$nthread"
		build/linear -i 0 -a `expr $(nproc) - 1` -I $i \
			-n 100 -t pi | grep Average | awk '{print $3}'
	done
}

test_3_integer_add()
{
	for ((i=4; i<=$(nproc); i+=4))
	do
		Ncpu="`expr $i - 1`"
		max_cpu=`expr $i - 1`
		Msec=`build/linear -N 0 -a $max_cpu -t arithmetic,integer,div -n 10 |\
			grep Average | awk '{print $3}'`
		echo -e "$i \t $Msec"
	done
}

test_3_float_add()
{
	for ((i=4; i<=$(nproc); i+=4))
	do
		Ncpu="`expr $i - 1`"
		max_cpu=`expr $i - 1`
		Msec=`build/linear -N 0 -a $max_cpu -t arithmetic,float,div -n 10 |\
			grep Average | awk '{print $3}'`
		echo -e "$i \t $Msec"
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
		Msec=`build/linear -N 0 -a $max_cpu -t $type -n 1 |\
			grep Average | awk '{print $3}'`
		echo -e "$i \t $Msec"
	done
}

test_type arithmetic,integer,add
test_type arithmetic,integer,sub
test_type arithmetic,integer,mul
test_type arithmetic,integer,div

test_type arithmetic,float,add
test_type arithmetic,float,sub
test_type arithmetic,float,mul
test_type arithmetic,float,div

