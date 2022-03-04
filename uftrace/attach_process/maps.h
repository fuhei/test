/**
 * @file maps.h
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "list.h"
#include "process.h"
#include "rbtree.h"

struct process;

struct vma_struct {
    /**
     * [start, end)
     */
    unsigned long start, end, offset;
    unsigned int maj, min, inode;
    char perms[5], name_[256];

    struct process *task;
    
    /**
     * Node of struct process.vmas
     */
    struct list_head node;

    /**
     * Node of struct process.vmas_rb
     */
    struct rb_node rb_node;

    char *content;
};

extern struct list_head maps_vma_list;


int open_pid_mem(struct process *task);
int open_pid_maps(struct process *task);

struct vma_struct * alloc_vma(struct process *task);
int free_vma(struct process *task, struct vma_struct * vma);

struct vma_struct *find_vma(struct process *task, unsigned long vaddr);

int link_vma(struct process *task, struct vma_struct *vma);
int unlink_vma(struct process *task, struct vma_struct *vma);

void print_vma(struct vma_struct * vma);