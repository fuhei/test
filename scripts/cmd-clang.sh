#!/bin/bash

compile_llvm()
{
	clang++ -g -O3 `llvm-config --cxxflags --ldflags --system-libs --libs core` $*
}
