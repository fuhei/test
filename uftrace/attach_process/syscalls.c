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
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "syscalls.h"
#include "maps.h"
#include "log.h"
#include "compiler.h"

static unsigned char __unused __callrax[] = {
    0xff, 0xd0, /* call *%rax */
    0xcc, /* int3 */
};
unsigned char __syscall[] = {
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

int wait_for_stop(struct process *task, void *data)
{
    int ret, status = 0, pid = (int)(uintptr_t)data ?: task->pid;
    //siginfo_t siginfo;
    //ptrace(PTRACE_GETSIGINFO, pid, 0, &siginfo);

	while (1) {
		ret = ptrace(PTRACE_CONT, pid, NULL, (void *)(uintptr_t)status);
		if (ret < 0) {
			lerror("ptrace(PTRACE_CONT, %d, ...) %s\n",
                pid, strerror(ESRCH));
#if 1
            if (errno == ESRCH) {
                /* tracee is dead, PTRACE_CONT return No such process */
                ret = waitpid(pid, &status, __WALL | WNOHANG);
                /* ret can still be 0 here! */
            }
			return -1;
#else
            continue;
#endif
		}

		ret = waitpid(pid, &status, __WALL);
		if (ret < 0) {
			lerror("can't wait tracee %d\n", pid);
			return -1;
		}
        printf("-------------------\n");
		if (WIFSTOPPED(status))  {
            break;
            printf("-------------------0\n");
			if (WSTOPSIG(status) == SIGSTOP ||
			    WSTOPSIG(status) == SIGTRAP) {
                printf("-------------------1\n");
                
				break;
            }
			status = WSTOPSIG(status);
			continue;
		}

		status = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
	}
    return 0;
}

static int remote_syscall(struct process *task, int nr,
        unsigned long arg1, unsigned long arg2, unsigned long arg3,
        unsigned long arg4, unsigned long arg5, unsigned long arg6,
        unsigned long *res)
{
    int ret;
    struct user_regs_struct old_regs, regs, my_regs;

    SYSCALL_REGS_PREPARE(my_regs, nr, arg1, arg2, arg3, arg4, arg5, arg6);
    
    unsigned char orig_code[sizeof(__syscall)];
    unsigned long libc_base = task->libc_base->start;

    /* 1. Get old register */
    ret = ptrace(PTRACE_GETREGS, task->pid, NULL, &old_regs);
    if (ret == -1) {
        lerror("ptrace(PTRACE_GETREGS, %d, ...) failed, %s\n",
            task->pid, strerror(errno));
    }

    /* 2. Read original code from libc */
    ret = pread(task->memfd, (void*)orig_code, sizeof(__syscall), libc_base);
    if (ret <= 0) {
        lerror("pread(%d, ...) failed\n", task->memfd);
        return -1;
    }

    /* 3. Write syscall instruments to libc */
    ret = pwrite(task->memfd, __syscall, sizeof(__syscall), libc_base);
    if (ret <= 0) {
        lerror("pwrite(%d, ...) failed\n", task->memfd);
        goto poke_back;
    }

    /* 4.  */
    regs = old_regs;

    /* 5. Execute from syscall, see step 3. */
    regs.rsp = libc_base;

    copy_regs(&regs, &my_regs);

    /* 6. Set target process run from libc(syscall) */
    ret = ptrace(PTRACE_SETREGS, task->pid, NULL, &regs);
    if (ret == -1) {
        lerror("ptrace(PTRACE_SETREGS, %d, ...) failed, %s\n",
            task->pid, strerror(errno));
    }

    /* Run target process */
    ret = wait_for_stop(task, NULL);
	if (ret < 0) {
		lerror("failed call to func\n");
		goto poke_back;
	}

    ret = ptrace(PTRACE_GETREGS, task->pid, NULL, &regs);
    if (ret == -1) {
        lerror("ptrace(PTRACE_GETREGS, %d, ...) failed, %s\n",
            task->pid, strerror(errno));
    }

    ret = ptrace(PTRACE_SETREGS, task->pid, NULL, &old_regs);
    if (ret == -1) {
        lerror("ptrace(PTRACE_SETREGS, %d, ...) failed, %s\n",
            task->pid, strerror(errno));
    }

    my_regs = regs;
    *res = regs.rax;

poke_back:
    ret = pwrite(task->memfd, &orig_code, sizeof(__syscall), libc_base);
    if (ret <= 0) {
        lerror("pwrite(%d, ...) failed\n", task->memfd);
        goto poke_back;
    }
    return ret;
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

    ret = remote_syscall(task,
            __NR_mmap, addr, length, prot, flags, fd, offset, &result);
    if (ret < 0) {
        return 0;
    }
    return result;
}