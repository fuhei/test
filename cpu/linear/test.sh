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

test_2_interval
