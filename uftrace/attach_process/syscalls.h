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

unsigned long
remote_mmap(struct process *task,
            unsigned long addr,
            size_t length,
            int prot,
            int flags,
            int fd,
            off_t offset);