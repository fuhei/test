#!/bin/bash

for ((i=0;i<$(nproc);i++))
do
	build/linear -a $i -n 100 | grep Average | awk '{print $3}'
done
