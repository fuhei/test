/**
 * @file process.c
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
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>
#include <libgen.h>

#include "process.h"
#include "maps.h"
#include "file_utils.h"


static int __create_vmas(struct process *task)
{
    if (!list_empty(&task->vmas)) {
	    fprintf(stderr, "Already load vmas\n");
        return -1;
    }
    do {
		unsigned long start, end, offset;
		unsigned int maj, min, inode;
		char perms[5], name_[256];
		int r;
		char line[1024];
		struct vma_struct *vma;

		if (!fgets(line, sizeof(line), task->mapsfp))
			break;

		r = sscanf(line, "%lx-%lx %s %lx %x:%x %d %255s",
			   &start, &end, perms, &offset,
			   &maj, &min, &inode, name_);
		if (r <= 0) {
			fprintf(stderr, "sscanf failed.\n");
            return -1;
		}

		vma = alloc_vma(task);
		vma->start = start;
		vma->end = end;
		memcpy(vma->perms, perms, sizeof(vma->perms));
		vma->offset = offset;
		vma->maj = maj;
		vma->min = min;
		vma->inode = inode;
		strncpy(vma->name_, name_, sizeof(vma->name_));

		if (!strcmp(name_, task->comm)) {
			vma->type = VMA_SELF;
		} else if (!strncmp(basename(name_), "libc", 4) ||
			!strncmp(basename(name_), "libssp", 6)) {
			vma->type = VMA_LIBC;
		} else if (!strcmp(name_, "[heap]")) {
			vma->type = VMA_HEAP;
		} else if (!strncmp(basename(name_), "ld-linux", 8)) {
			vma->type = VMA_LD;
		} else if (!strcmp(name_, "[stack]")) {
			vma->type = VMA_STACK;
		} else if (!strcmp(name_, "[vvar]")) {
			vma->type = VMA_VVAR;
		} else if (!strcmp(name_, "[vdso]")) {
			vma->type = VMA_VDSO;
		} else if (!strcmp(name_, "[vsyscall]")) {
			vma->type = VMA_VSYSCALL;
		} else if (!strncmp(basename(name_), "lib", 3)) {
			vma->type = VMA_LIB_DONT_KNOWN;
		} else if (strlen(name_) == 0) {
			vma->type = VMA_ANON;
		} else {
			vma->type = VMA_NONE;
		}

        link_vma(task, vma);

		vma->content = malloc(vma->end - vma->start);
		assert(vma->content && "Malloc vma->content failed");
		r = pread(task->memfd, vma->content, vma->end - vma->start, (off_t)vma->start);
		if (r <= 0) {
			fprintf(stderr, "pread failed in %s.\n", vma->name_);
            return -1;
		}
	} while (1);

    return 0;
}

static int __load_vmas(struct process *task)
{
    int r;
	struct vma_struct *vma = NULL;
    list_for_each_entry(vma, &task->vmas, node) {
		vma->content = malloc(vma->end - vma->start);
		assert(vma->content && "Malloc vma->content failed");
		r = pread(task->memfd, vma->content, vma->end - vma->start, (off_t)vma->start);
		if (r <= 0) {
			fprintf(stderr, "pread failed in %s.\n", vma->name_);
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

static int __open_comm_elf(struct process *task)
{
    int ret;
    ret = mmap_file(task->comm, &task->elf_file.file_map, &task->elf_file.file_len);
    if (ret != 0) {
        fprintf(stderr, "Fail mmap file %s\n", task->comm);
        return -1;
    }
	if (check_elf_ehdr(task->elf_file.file_map) != 0) {
		munmap_file(task->elf_file.file_map, task->elf_file.file_len);
		task->elf_file.file_map = NULL;
		task->elf_file.file_len = 0;
		return -1;
	}

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
	assert(p && "invalide process pointer.");
	p->pid = pid;

	if (process_exist(pid) != 0) {
		fprintf(stderr, "process %d not exist.\n", pid);
		return -1;
	}
	if (__get_comm(p) != 0) {
		fprintf(stderr, "get %d comm failed.\n", pid);
		return -1;
	}
	__open_comm_elf(p);
	if (args->check_trace_func &&
		check_elf_ftrace_function(p->elf_file.file_map) != 0) {
		fprintf(stderr, "Process %d(%s) not support ftrace.\n"\
						"Maybe compile source code with `-pg'.\n", p->pid, p->comm);
		return -1;
	}

    list_init(&p->vmas);
    rb_init(&p->vmas_rb);

	p->memfd = open_pid_mem(p);
	if (p->memfd <= 0) {
		fprintf(stderr, "open %d mem failed.\n", p->pid);
		return -1;
	}

	p->mapsfd = open_pid_maps(p);
	if (p->mapsfd <= 0) {
		fprintf(stderr, "open %d maps failed.\n", p->pid);
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
		fprintf(stderr, "fdopen %d failed.\n", p->mapsfd);
		close(p->memfd);
		close(p->mapsfd);
		return -1;
	}

    __create_vmas(p);
    __load_vmas(p);


	return 0;
}

int close_process(struct process *p)
{
	if (p->elf_file.file_map && p->elf_file.file_len)
		munmap_file(p->elf_file.file_map, p->elf_file.file_len);

    return 0;
}

int process_attach(struct process *task)
{
	long rv;
	rv = ptrace(PTRACE_ATTACH, task->pid, NULL, NULL);
	if (rv != 0) {
		fprintf(stderr, "Attach %d failed.(%s)\n", task->pid, task->comm);
	}
    return rv;
}

int process_detach(struct process *task)
{
	long rv;
	rv = ptrace(PTRACE_DETACH, task->pid, NULL, NULL);
	if (rv != 0) {
		fprintf(stderr, "Detach %d failed.(%s)\n", task->pid, task->comm);
	}
    return rv;
}