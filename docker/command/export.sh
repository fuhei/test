#!/bin/bash

# 导出和导入容器
# 导出容器
# 如果要导出本地某个容器，可以使用 docker export 命令。
docker export 1e560fca3906 > ubuntu.tar
