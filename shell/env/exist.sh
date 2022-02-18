#!/bin/bash

# 检测环境变量是否存在
if [ -z $JAVA_HOME ];then
	echo "JAVA_HOME not exists"
else
	echo "JAVA_HOME = $JAVA_HOME"
fi
