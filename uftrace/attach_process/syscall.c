/**
 * @file syscall.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>

#include "process.h"

int upatch_arch_execute_remote_func(struct process *task,
            const unsigned char *code,
            size_t codelen,
            struct user_regs_struct *pregs,
            void *data)
{
    //TODO
    return 0;
}

long remote_syscall(struct process *task)
{
    //TODO
    return 0;
}