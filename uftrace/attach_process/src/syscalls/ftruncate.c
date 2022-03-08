/**
 * @file ftruncate.c
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

int remote_ftruncate(struct process *task, int remote_fd, off_t length)
{
    int ret;
    unsigned long result;
    ret = remote_syscall(task,
            __NR_ftruncate, remote_fd, length, 0, 0, 0, 0, &result);
    if (ret < 0) {
        return 0;
    }
    return result;
}
