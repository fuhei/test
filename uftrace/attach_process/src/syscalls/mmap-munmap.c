/**
 * @file mmap-munmap.c
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


unsigned long
remote_mmap(struct process *task,
            unsigned long addr, size_t length, int prot, int flags,
            int fd, off_t offset)
{
    int ret;
    unsigned long result;

    ret = remote_syscall(task,
            __NR_mmap, addr, length, prot, flags, fd, offset, &result);
    if (ret < 0) {
        return 0;
    }
    return result;
}

int remote_munmap(struct process *task, unsigned long addr, size_t length)
{
    int ret;
    unsigned long result;

    ret = remote_syscall(task,
            __NR_munmap, addr, length, 0, 0, 0, 0, &result);
    if (ret < 0) {
        return 0;
    }
    return result;
}

unsigned long remote_malloc(struct process *task, size_t length)
{
	unsigned long remote_addr;
    remote_addr = remote_mmap(task,
				0UL, length,
				PROT_READ | PROT_WRITE | PROT_EXEC,
				MAP_PRIVATE, -1, 0);
    if (remote_addr == (unsigned long)MAP_FAILED) {
        lerror("Remote malloc failed, %d\n", remote_addr);
        return 0UL;
    }
    return remote_addr;
}

int remote_free(struct process *task, unsigned long addr, size_t length)
{
    return remote_munmap(task, addr, length);
}