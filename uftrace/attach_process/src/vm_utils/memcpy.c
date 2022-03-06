/**
 * @file memcpy.c
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
#include <errno.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "process.h"
#include "maps.h"
#include "log.h"
#include "file_utils.h"


int memcpy_from_remote(struct process *task,
		void *dst, unsigned long remote_src, ssize_t size)
{
	int ret;
	ret = pread(task->memfd, dst, size, remote_src);
    if (ret <= 0) {
        lerror("pread(%d, ...) failed\n", task->memfd);
        return -1;
    }
	return ret;
}

int memcpy_to_remote(struct process *task,
		unsigned long remote_dst, void *src, ssize_t size)
{
	int ret;
	ret = pwrite(task->memfd, src, size, remote_dst);
    if (ret <= 0) {
        lerror("pwrite(%d, ...) failed\n", task->memfd);
        return -1;
    }
	return ret;
}