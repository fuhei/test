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
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "syscalls.h"
#include "maps.h"
#include "log.h"
#include "compiler.h"

#if defined(__x86_64__)
#include "arch/x86_64/regs.h"
#elif defined(__aarch64__)
#include "arch/aarch64/regs.h"
#endif


static void copy_regs(struct user_regs_struct *dst, struct user_regs_struct *src)
{
#define COPY_REG(x) dst->x = src->x
#if defined(__x86_64__)
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
#elif defined(__aarch64__)
    COPY_REG(regs[0]);
    COPY_REG(regs[1]);
    COPY_REG(regs[2]);
    COPY_REG(regs[3]);
    COPY_REG(regs[4]);
    COPY_REG(regs[5]);
    COPY_REG(regs[8]);
    COPY_REG(regs[29]);
    COPY_REG(regs[9]);
    COPY_REG(regs[10]);
    COPY_REG(regs[11]);
    COPY_REG(regs[12]);
    COPY_REG(regs[13]);
    COPY_REG(regs[14]);
    COPY_REG(regs[15]);
    COPY_REG(regs[16]);
    COPY_REG(regs[17]);
    COPY_REG(regs[18]);
    COPY_REG(regs[19]);
    COPY_REG(regs[20]);
#else
# error "Unsupport architecture"
#endif

#undef COPY_REG
}

int wait_for_stop(struct process *task, void *data)
{
    int ret, status = 0, pid = (int)(uintptr_t)data ?: task->pid;
    //siginfo_t siginfo;
    //ptrace(PTRACE_GETSIGINFO, pid, 0, &siginfo);

	while (1) {
		ret = ptrace(PTRACE_CONT, pid, NULL, (void *)(uintptr_t)status);
		if (ret < 0) {
            print_vma(task->libc_base);
			lerror("ptrace(PTRACE_CONT, %d, ...) %s\n",
                pid, strerror(ESRCH));
			return -1;
		}

		ret = waitpid(pid, &status, __WALL);
		if (ret < 0) {
			lerror("can't wait tracee %d\n", pid);
			return -1;
		}
		if (WIFSTOPPED(status))  {
			if (WSTOPSIG(status) == SIGSTOP ||
			    WSTOPSIG(status) == SIGTRAP) {
				break;
            }
            if (WSTOPSIG(status) == SIGSEGV) {
                lerror("Child process %d segment fault.\n", pid);
            }
			status = WSTOPSIG(status);
			continue;
		}

		status = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
	}
    return 0;
}

int
remote_syscall(struct process *task, int nr,
        unsigned long arg1, unsigned long arg2, unsigned long arg3,
        unsigned long arg4, unsigned long arg5, unsigned long arg6,
        unsigned long *res)
{
    int ret;
    struct user_regs_struct old_regs, regs, __unused syscall_regs;
    unsigned char __syscall[] = {SYSCALL_INSTR};

    SYSCALL_REGS_PREPARE(syscall_regs, nr, arg1, arg2, arg3, arg4, arg5, arg6);

    unsigned char orig_code[sizeof(__syscall)];
    unsigned long libc_base = task->libc_base->start;

    /* 1. Get old register */
    ret = ptrace(PTRACE_GETREGS, task->pid, NULL, &old_regs);
    if (ret == -1) {
        lerror("ptrace(PTRACE_GETREGS, %d, ...) failed, %s\n",
            task->pid, strerror(errno));
    }

    /* 2. Read original code from libc */
    memcpy_from_remote(task, orig_code, libc_base, sizeof(__syscall));

    /* 3. Write syscall instruments to libc */
    memcpy_to_remote(task, libc_base, __syscall, sizeof(__syscall));

    /* 4.  */
    regs = old_regs;

    /* 5. Execute from syscall, see step 3. */
    SYSCALL_IP(regs) = libc_base;

    copy_regs(&regs, &syscall_regs);

    /* 6. Set target process run from (syscall) */
    ret = ptrace(PTRACE_SETREGS, task->pid, NULL, &regs);
    if (ret == -1) {
        lerror("ptrace(PTRACE_SETREGS, %d, ...) failed, %s\n",
            task->pid, strerror(errno));
    }

    /* 7. Run target process */
    ret = wait_for_stop(task, NULL);
	if (ret < 0) {
		lerror("failed call to func\n");
		goto poke_back;
	}

    /* 8. Get syscall return regs */
    ret = ptrace(PTRACE_GETREGS, task->pid, NULL, &regs);
    if (ret == -1) {
        lerror("ptrace(PTRACE_GETREGS, %d, ...) failed, %s\n",
            task->pid, strerror(errno));
    }

    /* 9. Restorage original registers */
    ret = ptrace(PTRACE_SETREGS, task->pid, NULL, &old_regs);
    if (ret == -1) {
        lerror("ptrace(PTRACE_SETREGS, %d, ...) failed, %s\n",
            task->pid, strerror(errno));
    }

    /* 10. Get return value */
    syscall_regs = regs;
    *res = SYSCALL_RET(syscall_regs);

poke_back:
    /* 11. Poke libc code back */
    memcpy_to_remote(task, libc_base, orig_code, sizeof(__syscall));
    return ret;
}


