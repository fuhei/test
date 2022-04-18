#!/bin/bash

function clean_out() {
	QA2 "Remove all *.out under [$PWD] "
	if [ $? != 0 ]; then
		return 1
	fi
	filename=("*.out")
	for name in ${filename[@]}; do
		#echo $name
		find . -name $name -type f -print -exec rm -rf {} \;
	done 
	return 0
}

