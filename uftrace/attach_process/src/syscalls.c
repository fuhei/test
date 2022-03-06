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


#define SYSCALL_REGS_PREPARE(regs, nr, p1, p2, p3, p4, p5, p6)  \
        regs.rax = (unsigned long)nr;   \
        regs.rdi = p1;  \
        regs.rsi = p2;  \
        regs.rdx = p3;  \
        regs.r10 = p4;  \
        regs.r8 = p5;   \
        regs.r9 = p6;   
#define SYSCALL_RET(regs)   regs.rax

void copy_regs(struct user_regs_struct *dst, struct user_regs_struct *src)
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

static int
remote_syscall(struct process *task, int nr,
        unsigned long arg1, unsigned long arg2, unsigned long arg3,
        unsigned long arg4, unsigned long arg5, unsigned long arg6,
        unsigned long *res)
{
    int ret;
    struct user_regs_struct old_regs, regs, __unused syscall_regs;
    unsigned char __syscall[] = {
        0x0f, 0x05, /* syscall */
        0xcc, /* int3 */
    };

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
    regs.rip = libc_base;

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

/**
 * @brief Remote open(2)
 * 
 * @param task 
 * @param pathname 
 * @param flags 
 * @param mode 
 * @return int 
 */
int remote_open(struct process *task, char *pathname, int flags, mode_t mode)
{
    char maybeislink[PATH_MAX], path[PATH_MAX];
    int ret;
    unsigned long result;

    unsigned long remote_fileaddr;
    ssize_t remote_filename_len = 0;

    if (!(flags|O_CREAT)) {
        ret = readlink(pathname, maybeislink, sizeof(maybeislink));                                                                           
        if (ret < 0) {
            lwarning("readlink(3) failed.\n");
            return -1;
        }
        maybeislink[ret] = '\0';
        if (!realpath(maybeislink, path)) {
            lwarning("realpath(3) failed.\n");
            return -1;
        }
        ldebug("%s -> %s -> %s\n", pathname, maybeislink, path);
        pathname = path;
    }
    remote_filename_len = strlen(pathname) + 1;

	remote_fileaddr = remote_mmap(task,
				0UL, remote_filename_len,
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    memcpy_to_remote(task, remote_fileaddr, pathname, remote_filename_len);

    ret = remote_syscall(task,
            __NR_open, remote_fileaddr, flags, mode, 0, 0, 0, &result);
    
	remote_munmap(task, remote_fileaddr, remote_filename_len);

    return result;
}

/**
 * @brief Remote close(2)
 * 
 * @param task 
 * @param remote_fd 
 * @return int 
 */
int remote_close(struct process *task, int remote_fd)
{
    int ret;
    unsigned long result;
    ret = remote_syscall(task,
            __NR_close, remote_fd, 0, 0, 0, 0, 0, &result);
    if (ret < 0) {
        return 0;
    }
    return result;
}

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

	sleep(5);
	ldebug("unmmap. %lx\n", map_v);
	remote_munmap(task, map_v, map_len);

close_ret:
	remote_close(task, map_fd);

    return 0;
}