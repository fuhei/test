/**
 * @file attach-detach.c
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

int process_attach(struct process *task)
{
	int ret;
	int status;
	pid_t pid = task->pid;
	ret = ptrace(PTRACE_ATTACH, task->pid, NULL, NULL);
	if (ret != 0) {
		lerror("Attach %d failed.(%s)\n", pid, task->comm);
		return ret;
	}
	do {
		ret = waitpid(pid, &status, __WALL);
		if (ret < 0) {
			lerror("can't wait for pid %d\n", pid);
			return -1;
		}
		ret = 0;

		/* We are expecting SIGSTOP */
		if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGSTOP)
			break;

		/* If we got SIGTRAP because we just got out of execve, wait
		 * for the SIGSTOP
		 */
		if (WIFSTOPPED(status))
			status = (WSTOPSIG(status) == SIGTRAP) ? 0 : WSTOPSIG(status);
		else if (WIFSIGNALED(status))
			/* Resend signal */
			status = WTERMSIG(status);

		ret = ptrace(PTRACE_CONT, pid, NULL, (void *)(uintptr_t)status);
		if (ret < 0) {
			lerror("can't cont tracee\n");
			return -1;
		}
	} while (1);

	task->running = false;

    return ret;
}

int process_detach(struct process *task)
{
	long rv;
	rv = ptrace(PTRACE_DETACH, task->pid, NULL, NULL);
	if (rv != 0) {
		lerror("Detach %d failed.(%s)\n", task->pid, task->comm);
		return -errno;
	}

	task->running = true;
	task->pid = 0;
    return rv;
}