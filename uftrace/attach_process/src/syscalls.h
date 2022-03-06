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

#include <sys/mman.h>
#include <sys/types.h>
#include "process.h"


int test_syscall_mmap(struct process *task);

unsigned long
remote_mmap(struct process *task,
            unsigned long addr, size_t length, int prot, int flags,
            int fd, off_t offset);
int remote_munmap(struct process *task, unsigned long addr, size_t length);

int remote_open(struct process *task, char *pathname, int flags, mode_t mode);
int remote_close(struct process *task, int remote_fd);

int remote_ftruncate(struct process *task, int remote_fd, off_t length);