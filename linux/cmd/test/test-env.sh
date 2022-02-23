#!/bin/bash

export RTOAX="/usr/bin/clang"

# 这么检测好像不管用
# test -z $RTOAX && export ECHO_TEST="echo me"

if [ -z $RTOAX ];then
	echo "RTOAX not exists"
else
	echo "RTOAX = $RTOAX"
	result=$(echo $RTOAX | grep "clang")
	if [ -n "$result" ]; then
		echo "use Clang"
	else
		echo "not use Clang"
	fi
fi

echo "echo: $ECHO_TEST"

