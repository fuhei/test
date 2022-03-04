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
#include <sys/syscall.h>
#include <unistd.h>

#include "syscalls.h"
#include "maps.h"
#include "compiler.h"

static unsigned char __unused __callrax[] = {
    0xff, 0xd0, /* call *%rax */
    0xcc, /* int3 */
};
unsigned char syscall[] = {
    0x0f, 0x05, /* syscall */
    0xcc, /* int3 */
};

# define SYSCALL_REGS_PREPARE(regs, nr, p1, p2, p3, p4, p5, p6)  \
        regs.rax = (unsigned long)nr;   \
        regs.rdi = p1;  \
        regs.rsi = p2;  \
        regs.rdx = p3;  \
        regs.r10 = p4;  \
        regs.r8 = p5;   \
        regs.r9 = p6;   
# define SYSCALL_RET(regs)   regs.rax

void copy_regs(struct user_regs_struct *dst,
              struct user_regs_struct *src)
{
#define COPY_REG(x) dst->x = src->x
    COPY_REG(r15);
    COPY_REG(r14);
    COPY_REG(r13);
    COPY_REG(r12);
    COPY_REG(rbp);
    COPY_REG(rbx);
    COPY_REG(r11);
    COPY_REG(r10);
    COPY_REG(r9);
    COPY_REG(r8);
    COPY_REG(rax);
    COPY_REG(rcx);
    COPY_REG(rdx);
    COPY_REG(rsi);
    COPY_REG(rdi);
#undef COPY_REG
}

void print_user_regs_struct(struct user_regs_struct *regs)
{
    //TODO
}

int wait_for_stop()
{

}

static int remote_syscall(struct process *task, int nr,
        unsigned long arg1, unsigned long arg2, unsigned long arg3,
        unsigned long arg4, unsigned long arg5, unsigned long arg6,
        unsigned long *res)
{
    int ret;
    struct user_regs_struct orig_regs, regs, my_regs;

    
    SYSCALL_REGS_PREPARE(my_regs, nr, arg1, arg2, arg3, arg4, arg5, arg6);
    
    unsigned char orig_code[sizeof(syscall)];
    unsigned long libc_base = task->libc_base->start;


    ret = ptrace(PTRACE_GETREGS, task->pid, NULL, &orig_regs);
    if (ret == -1) {
        fprintf(stderr, "ptrace(PTRACE_GETREGS, %d, ...) failed\n", task->pid);
    }

    ret = pread(task->memfd, &orig_code, sizeof(syscall), libc_base);
    if (ret <= 0) {
        fprintf(stderr, "pread(%d, ...) failed\n", task->memfd);
        return -1;
    }
    ret = pwrite(task->memfd, syscall, sizeof(syscall), libc_base);
    if (ret <= 0) {
        fprintf(stderr, "pwrite(%d, ...) failed\n", task->memfd);
        return -1;
    }

    regs = orig_regs;
    regs.rsp = libc_base;

    copy_regs(&regs, &my_regs);
    ret = ptrace(PTRACE_SETREGS, task->pid, NULL, &regs);

    /* wait func, run target process */

    ret = ptrace(PTRACE_GETREGS, task->pid, NULL, &regs);
    ret = ptrace(PTRACE_SETREGS, task->pid, NULL, &orig_regs);
    
    my_regs = regs;

    ret = pwrite(task->memfd, &orig_code, sizeof(syscall), libc_base);
    //TODO
    return 0;
}

unsigned long remote_mmap(struct process *task,
                        unsigned long addr,
                        size_t length,
                        int prot,
                        int flags,
                        int fd,
                        off_t offset)
{
    int ret;
    unsigned long result;

    ret = remote_syscall(task, __NR_mmap, addr, length, prot, flags, fd, offset, &result);
    if (ret < 0) {
        return 0;
    }
    return result;
}