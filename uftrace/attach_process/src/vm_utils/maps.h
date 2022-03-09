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
#include <sys/types.h>

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
    VMA_FTRACE_TRAMPOLINE,  /* ftrace trampoline */
    VMA_PATCH_OBJ, /* User space patch object file */
    VMA_TYPE_NUM,
};
#define VMA_TYPE_NAME(t) __VMA_TYPE_NAME[t]
static const char __unused *__VMA_TYPE_NAME[] = {
"Unknown", "Self", "libc", "heap", "ld", "stack", "vvar", "vDSO", "vsyscall", "UnknownLib", "Anon",
"Trampoline", "PatchObj", NULL
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

    /* Read from /proc/PID/mem */
    char *dup_remote_mem;

    /* Local file mmap */
    char *obj_local;
    ssize_t obj_size;
};

int test_maps_vma(struct process *task);

//extern struct list_head maps_vma_list;

unsigned int __perms2prot(char *perms);

int open_pid_mem(struct process *task);
int open_pid_maps(struct process *task);

int remote_insert_object(struct process *task, char *objreal, char *objcopy, ssize_t size, enum vma_type type);

struct vma_struct * alloc_vma();
int free_vma(struct vma_struct * vma);
int link_vma(struct process *task, struct vma_struct *vma);
int unlink_vma(struct process *task, struct vma_struct *vma);
int for_each_maps_vma(struct process *task, int (*handler)(struct vma_struct * tmpvma, void *arg), void *arg);
int for_each_process_vma(struct process *task, int (*handler)(struct vma_struct * vma, void *arg), void *arg);

struct vma_struct *find_vma(struct process *task, unsigned long vaddr);


void print_vma(struct vma_struct * vma);