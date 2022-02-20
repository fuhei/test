#!/bin/bash

# Hello
docker run centos /bin/echo "Hello"

# 以下命令使用 ubuntu 镜像启动一个容器，参数为以命令行模式进入该容器：
#-i: 交互式操作。
#-t: 终端。
#ubuntu: ubuntu 镜像。
#/bin/bash：放在镜像名后的是命令，这里我们希望有个交互式 Shell，因此用的是 /bin/bash。
docker run -ti centos /bin/bash

# 后台运行
# 在大部分的场景下，我们希望 docker 的服务是在后台运行的，我们可以过 -d 指定容器的运行模式。
docker run -itd --name ubuntu-test ubuntu /bin/bash

# 在使用 -d 参数时，容器启动后会进入后台。此时想要进入容器，
# see attach.sh and exec.sh
