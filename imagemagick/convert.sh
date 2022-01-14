#!/bin/bash

for f in `cat list.txt`
do 
	# 1080x1080
	size=${f:18:(13-4)}
	convert a.jpg -crop $size $f
done
