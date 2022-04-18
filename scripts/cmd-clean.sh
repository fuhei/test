#!/bin/bash

function clean_out() {
	QA2 "Remove all *.out under [$PWD] "
	filename=("*.out")
	for name in ${filename[@]}; do
		#echo $name
		find . -name $name -type f -print -exec rm -rf {} \;
	done 
	return 0
}

