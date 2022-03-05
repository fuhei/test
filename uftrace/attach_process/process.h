/**
 * @file process.h
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#include "list.h"
#include "rbtree.h"
#include "elf_utils.h"

struct process {
	pid_t pid;
	int memfd;
	int mapsfd;
	FILE *mapsfp;

    bool running;
    /**
     * List of struct vma_struct.node
     */
    struct list_head vmas;

    /**
     * Tree of struct vma_struct.rb_node
     */
    struct rb_root vmas_rb;

    /* Whole name of process, such as /usr/bin/ls */
    char *comm;

    /**
     * Use for:
     * 1. storage custom instruments.
     * ...
     */
    union {
        struct vma_struct *libc_base;
        /* some times, libc-xxx.so is link to libssp.so.x. */
        struct vma_struct *libssp;
    };

    struct elf_file_struct elf_file;
};

int process_exist(pid_t pid);
int open_process(struct process *p, pid_t pid, struct args_opts *args);
int close_process(struct process *p);

int process_attach(struct process *task);
int process_detach(struct process *task);

int memcpy_from_remote(struct process *task,
		void *dst, unsigned long remote_src, ssize_t size);
int memcpy_to_remote(struct process *task,
		unsigned long remote_dst, void *src, ssize_t size);