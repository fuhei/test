/**
 * @file test.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "syscalls.h"
#include "maps.h"
#include "log.h"
#include "compiler.h"

int test_syscall_mmap(struct process *task)
{
    int ret = 0;
	unsigned long map_v;
	ssize_t map_len = 8192;
	int __unused map_fd;

	map_fd = remote_open(task, "todo.txt", O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (map_fd <= 0) {
        lwarning("remote open failed.\n");
        return -1;
    }
	ldebug("New open. %d\n", map_fd);
	ret = remote_ftruncate(task, map_fd, map_len);
    if (ret != 0) {
        lwarning("remote ftruncate failed.\n");
        goto close_ret;
    }
	map_v = remote_mmap(task,
				0UL, map_len,
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE, map_fd, 0);
	ldebug("New mmap. %lx\n", map_v);

	ldebug("unmmap. %lx\n", map_v);
	remote_munmap(task, map_v, map_len);

close_ret:
	remote_close(task, map_fd);

    return 0;
}