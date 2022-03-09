/**
 * @file prctl.c
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
#include <sys/prctl.h>

#include "syscalls.h"
#include "maps.h"
#include "log.h"
#include "compiler.h"

int remote_prctl(struct process *task, int option, unsigned long arg2, unsigned long arg3,
                 unsigned long arg4, unsigned long arg5)
{

    int ret;
    unsigned long result;

    ret = remote_syscall(task,
            __NR_prctl, option, arg2, arg3, arg4, arg5, 0, &result);
    if (ret < 0) {
        return 0;
    }
    return result;
}

#if 0
/**
 * LWN: Not-so-anonymous virtual memory areas(https://lwn.net/Articles/867818/)
 * LWN：让vma的匿名页不再匿名！(https://blog.csdn.net/Linux_Everything/article/details/120426325)
 */
int remote_prctl_set_vma_anon_name(struct process *task, unsigned long start, unsigned long len, char *name)
{
    int ret;
    unsigned long result;

    unsigned long remote_addr;
    ssize_t remote_len = 0;
    remote_len = strlen(name) + 1;

    remote_addr = remote_mmap(task,
				0UL, remote_len,
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memcpy_to_remote(task, remote_addr, name, remote_len);

    ret = remote_prctl(task, PR_SET_VMA, PR_SET_VMA_ANON_NAME, start, len, remote_addr);

    remote_munmap(task, remote_faddr, remote_len);

    return ret;
}
#endif