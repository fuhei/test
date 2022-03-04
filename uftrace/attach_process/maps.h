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

#include <stdbool.h>

#include "list.h"
#include "process.h"
#include "rbtree.h"
#include "compiler.h"

struct process;

enum vma_type {
    VMA_NONE,   /* None */
    VMA_SELF,   /* /usr/bin/ls, ... */
    VMA_LIBC,   /* /usr/lib64/libc.so.x */
    VMA_HEAP,   /* [heap] */
    VMA_LD,     /* /usr/lib64/ld-linux-xxxxx */
    VMA_STACK,  /* [stack] */
    VMA_VVAR,   /* [vvar] */
    VMA_VDSO,   /* [vdso] */
    VMA_VSYSCALL, /* [vsyscall] */
    VMA_LIB_DONT_KNOWN, /* Unknown Library */
    VMA_ANON,   /* No name */
    VMA_TYPE_NUM,
};
#define VMA_TYPE_NAME(t) __VMA_TYPE_NAME[t]
static const char __unused *__VMA_TYPE_NAME[] = {
"Unknown", "Self", "libc", "heap", "ld", "stack", "vvar", "vDSO", "vsyscall", "UnknownLib", "Anon",  NULL
};

struct vma_struct {
    /**
     * [start, end)
     */
    unsigned long start, end, offset;
    unsigned int maj, min, inode;
    char perms[5], name_[256];
#define PROT_FMT "%c%c%c"
#define PROT_ARGS(p) \
    (p & PROT_READ) ? 'r' : '-', \
    (p & PROT_WRITE) ? 'w' : '-', \
    (p & PROT_EXEC) ? 'e' : '-'
    unsigned int prot; /* parse from char perms[5] */

    enum vma_type type;

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