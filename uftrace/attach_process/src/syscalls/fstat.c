/**
 * @file fstat.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-09
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

/**
 * @brief
 *
 * @param task
 * @param remote_fd
 * @param statbuf
 * @return int
 *
 * Tracing with:
 * $ sudo bpftrace -e 'tracepoint:syscalls:sys_enter_newfstat{printf("comm = %s, fd = %d\n", comm, args->fd);}'
 */
int remote_fstat(struct process *task, int remote_fd, struct stat *statbuf)
{
    int ret;
    unsigned long remote_statbuf;
    unsigned long result;

    /* Alloc stat struct from remote */
    remote_statbuf = remote_mmap(task,
				0UL, sizeof(struct stat),
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    /* Call fstat(2) */
    ret = remote_syscall(task,
            __NR_fstat, remote_fd, remote_statbuf, 0, 0, 0, 0, &result);
    if (ret < 0) {
        lerror("fstat failed, ret %d, %ld\n", ret, result);
    }

    ret = memcpy_from_remote(task, statbuf, remote_statbuf, sizeof(struct stat));
    remote_munmap(task, remote_statbuf, sizeof(struct stat));

    return ret;
}