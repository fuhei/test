/**
 * @file open-close.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-03
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
#include <stdlib.h>
#include <libgen.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "process.h"
#include "maps.h"
#include "log.h"
#include "file_utils.h"

static int __create_vma_handler(struct vma_struct * tmpvma, void *arg)
{
	struct vma_struct *vma;
	struct process *task = (struct process *)arg;

	vma = alloc_vma();
	vma->start = tmpvma->start;
	vma->end = tmpvma->end;
	memcpy(vma->perms, tmpvma->perms, sizeof(vma->perms));
	vma->prot = __perms2prot(tmpvma->perms);
	vma->offset = tmpvma->offset;
	vma->maj = tmpvma->maj;
	vma->min = tmpvma->min;
	vma->inode = tmpvma->inode;
	strncpy(vma->name_, tmpvma->name_, sizeof(vma->name_));

	if (!strcmp(vma->name_, task->comm)) {
		vma->type = VMA_SELF;
	} else if (!strncmp(basename(vma->name_), "libc", 4) ||
		!strncmp(basename(vma->name_), "libssp", 6)) {
		vma->type = VMA_LIBC;
		linfo("libc base = %lx\n", task->libc_base);
		if (!task->libc_base &&
			vma->prot & PROT_EXEC) {
			task->libc_base = vma;
			linfo("libc base = %lx\n", task->libc_base);
		}
	} else if (!strcmp(vma->name_, "[heap]")) {
		vma->type = VMA_HEAP;
	} else if (!strncmp(basename(vma->name_), "ld-linux", 8)) {
		vma->type = VMA_LD;
	} else if (!strcmp(vma->name_, "[stack]")) {
		vma->type = VMA_STACK;
	} else if (!strcmp(vma->name_, "[vvar]")) {
		vma->type = VMA_VVAR;
	} else if (!strcmp(vma->name_, "[vdso]")) {
		vma->type = VMA_VDSO;
	} else if (!strcmp(vma->name_, "[vsyscall]")) {
		vma->type = VMA_VSYSCALL;
	} else if (!strncmp(basename(vma->name_), "lib", 3)) {
		vma->type = VMA_LIB_DONT_KNOWN;
	} else if (strlen(vma->name_) == 0) {
		vma->type = VMA_ANON;
	} else {
		vma->type = VMA_NONE;
	}
	link_vma(task, vma);
	return 0;
}

static int __create_vmas(struct process *task)
{
    if (!list_empty(&task->vmas)) {
	    lerror("Already load vmas\n");
        return -1;
    }

	for_each_maps_vma(task, __create_vma_handler, (void*)task);

    return 0;
}

static int __load_vmas(struct process *task)
{
    int r;
	struct vma_struct *vma = NULL;
    list_for_each_entry(vma, &task->vmas, node) {
		/* filter */
		switch (vma->type) {
			case VMA_NONE:
			case VMA_HEAP:
			case VMA_LD:
			case VMA_STACK:
			case VMA_VVAR:
			case VMA_VDSO:
			case VMA_VSYSCALL:
			case VMA_LIB_DONT_KNOWN:
			case VMA_ANON:
			case VMA_FTRACE_TRAMPOLINE:
			case VMA_PATCH_OBJ:
				vma->dup_remote_mem = NULL;
				continue;
			case VMA_SELF:
			case VMA_LIBC:
			default:
				break;
		}
		vma->dup_remote_mem = malloc(vma->end - vma->start);
		assert(vma->dup_remote_mem && "Malloc vma->dup_remote_mem failed");
		r = memcpy_from_remote(task, vma->dup_remote_mem, vma->start, vma->end - vma->start);
		if (r <= 0) {
			lerror("pread failed in %s.\n", vma->name_);
            return -1;
		}
	}
    return 0;
}

static int __get_comm(struct process *task)
{
    char path[128], realpath[128];
    ssize_t ret;

    snprintf(path, sizeof(path), "/proc/%d/exe", task->pid);
    ret = readlink(path, realpath, sizeof(realpath));
    if (ret < 0)
        return -1;
    realpath[ret] = '\0';
	task->comm = strdup(realpath);

    return 0;
}


int process_exist(pid_t pid)
{
    char proc_pid[] = "/proc/1234567890";
	snprintf(proc_pid, sizeof(proc_pid), "/proc/%d", pid);

	if (access(proc_pid, F_OK) == 0) {
		return 0;
	}

    return -1;
}

int open_process(struct process *p, pid_t pid, struct args_opts *args)
{
	int ret;

	assert(p && "invalide process pointer.");

	memset(p, 0x00, sizeof(struct process));

	p->pid = pid;

	if (process_exist(pid) != 0) {
		lerror("process %d not exist.\n", pid);
		return -1;
	}
	if (__get_comm(p) != 0) {
		lerror("get %d comm failed.\n", pid);
		return -1;
	}

	/* Open COMM file */
	ret = open_elf_file(p->comm, &p->elf_file);
	if (ret != 0) {
		return ret;
	}
	if (args->check_trace_func &&
		elf_check_ftrace_function(&p->elf_file) != 0) {
		lerror("Process %d(%s) not support ftrace.\n"\
						"Maybe compile source code with `-pg'.\n", p->pid, p->comm);
		return -1;
	}

    list_init(&p->vmas);
    rb_init(&p->vmas_rb);

	p->memfd = open_pid_mem(p);
	if (p->memfd <= 0) {
		lerror("open %d mem failed.\n", p->pid);
		return -1;
	}

	p->mapsfd = open_pid_maps(p);
	if (p->mapsfd <= 0) {
		lerror("open %d maps failed.\n", p->pid);
		close(p->memfd);
		return -1;
	}

	/**
	 * /proc/PID/maps
	 * 56028a405000-56028a433000 r--p 00000000 fd:00 70252                      /usr/bin/bash
	 */
	lseek(p->mapsfd, 0, SEEK_SET);
	p->mapsfp = fdopen(p->mapsfd, "r");
	if (p->mapsfp == NULL) {
		lerror("fdopen %d failed.\n", p->mapsfd);
		close(p->memfd);
		close(p->mapsfd);
		return -1;
	}

    __create_vmas(p);
    __load_vmas(p);

	p->running = true;

	return 0;
}

int close_process(struct process *p)
{
	if (p->elf_file.file_map && p->elf_file.file_len)
		munmap_file(p->elf_file.file_map, p->elf_file.file_len);

    return 0;
}

