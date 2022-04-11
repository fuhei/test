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
	for ((i=1; i<=$(nproc); i+=4))
	do
		#echo "`expr $i - 1`"
		build/linear -N 0 -a `expr $i - 1` -t arithmetic,integer,add -n 1 |\
			grep Average | awk '{print $3}'
	done
}

test_3_integer_add
