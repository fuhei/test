#!/bin/bash

repo=MyAppStream

dnf --repo=$repo \
	--repoformpath=$repo,https://mirrors.bfsu.edu.cn/centos-stream/9-stream/$repo/aarch64/debug/tree/ \
	reposync
