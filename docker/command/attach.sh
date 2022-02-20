#!/bin/bash

# 进入容器
# 在使用 -d 参数时，容器启动后会进入后台。此时想要进入容器，可以通过以下指令进入：
docker attach

# docker exec：推荐大家使用 docker exec 命令，因为此退出容器终端，不会导致容器的停止。

# attach 命令
# 下面演示了使用 docker attach 命令。
docker attach 1e560fca3906
