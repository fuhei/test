#!/bin/bash

# 导入容器快照
# 可以使用 docker import 从容器快照文件中再导入为镜像，
# 以下实例将快照文件 ubuntu.tar 导入到镜像 test/ubuntu:v1:
cat docker/ubuntu.tar | docker import - test/ubuntu:v1

# 此外，也可以通过指定 URL 或者某个目录来导入，例如：
docker import http://example.com/exampleimage.tgz example/imagerepo
