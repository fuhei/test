#!/bin/bash 

docker build -t test-dockerfile-entrypoint .
docker run test-dockerfile-entrypoint -i
