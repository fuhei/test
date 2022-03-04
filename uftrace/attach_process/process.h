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
    /**
     * List of struct vma_struct.node
     */
    struct list_head vmas;

    /**
     * Tree of struct vma_struct.rb_node
     */
    struct rb_root vmas_rb;

    char *comm;

    struct elf_file_struct elf_file;
};

int process_exist(pid_t pid);
int open_process(struct process *p, pid_t pid);
int close_process(struct process *p);

int process_attach(struct process *task);
int process_detach(struct process *task);