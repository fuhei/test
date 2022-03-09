/**
 * @file syscall.h
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <sys/mman.h>
#include <sys/types.h>
#include "process.h"


int test_syscall_mmap(struct process *task);

int
remote_syscall(struct process *task, int nr,
        unsigned long arg1, unsigned long arg2, unsigned long arg3,
        unsigned long arg4, unsigned long arg5, unsigned long arg6,
        unsigned long *res);

unsigned long
remote_mmap(struct process *task,
            unsigned long addr, size_t length, int prot, int flags,
            int fd, off_t offset);
int remote_munmap(struct process *task, unsigned long addr, size_t length);
unsigned long remote_malloc(struct process *task, size_t length);
int remote_free(struct process *task, unsigned long addr, size_t length);

int remote_open(struct process *task, char *pathname, int flags, mode_t mode);
int remote_close(struct process *task, int remote_fd);

int remote_ftruncate(struct process *task, int remote_fd, off_t length);

int remote_fstat(struct process *task, int remote_fd, struct stat *statbuf);

int remote_prctl(struct process *task, int option, unsigned long arg2, unsigned long arg3,
                 unsigned long arg4, unsigned long arg5);
int remote_prctl_set_vma_anon_name(struct process *task, unsigned long start, unsigned long len, char *name);