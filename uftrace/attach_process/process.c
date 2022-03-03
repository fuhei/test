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

#include "process.h"
#include "maps.h"

static int create_vmas(struct process *task)
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

static int load_vmas(struct process *task)
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

int open_process(struct process *p, pid_t pid)
{
    list_init(&p->vmas);
    rb_init(&p->vmas_rb);

	p->pid = pid;
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

    create_vmas(p);
    load_vmas(p);

	return 0;
}

int close_process(struct process *p)
{
    return 0;
}

int process_attach(struct process *task)
{
	long rv;
	rv = ptrace(PTRACE_ATTACH, task->pid, NULL, NULL);
	if (rv != 0) {
		fprintf(stderr, "Attach %d failed.\n", task->pid);
	}
    return rv;
}

int process_detach(struct process *task)
{
	long rv;
	rv = ptrace(PTRACE_DETACH, task->pid, NULL, NULL);
	if (rv != 0) {
		fprintf(stderr, "Detach %d failed.\n", task->pid);
	}
    return rv;
}