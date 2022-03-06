/**
 * @file open-close.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-03-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "syscalls.h"
#include "maps.h"
#include "log.h"
#include "compiler.h"

/**
 * @brief Remote open(2)
 * 
 * @param task 
 * @param pathname 
 * @param flags 
 * @param mode 
 * @return int 
 */
int remote_open(struct process *task, char *pathname, int flags, mode_t mode)
{
    char maybeislink[PATH_MAX], path[PATH_MAX];
    int ret;
    unsigned long result;

    unsigned long remote_fileaddr;
    ssize_t remote_filename_len = 0;

    if (!(flags|O_CREAT)) {
        ret = readlink(pathname, maybeislink, sizeof(maybeislink));                                                                           
        if (ret < 0) {
            lwarning("readlink(3) failed.\n");
            return -1;
        }
        maybeislink[ret] = '\0';
        if (!realpath(maybeislink, path)) {
            lwarning("realpath(3) failed.\n");
            return -1;
        }
        ldebug("%s -> %s -> %s\n", pathname, maybeislink, path);
        pathname = path;
    }
    remote_filename_len = strlen(pathname) + 1;

	remote_fileaddr = remote_mmap(task,
				0UL, remote_filename_len,
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    memcpy_to_remote(task, remote_fileaddr, pathname, remote_filename_len);

    ret = remote_syscall(task,
            __NR_open, remote_fileaddr, flags, mode, 0, 0, 0, &result);
    
	remote_munmap(task, remote_fileaddr, remote_filename_len);

    return result;
}

/**
 * @brief Remote close(2)
 * 
 * @param task 
 * @param remote_fd 
 * @return int 
 */
int remote_close(struct process *task, int remote_fd)
{
    int ret;
    unsigned long result;
    ret = remote_syscall(task,
            __NR_close, remote_fd, 0, 0, 0, 0, 0, &result);
    if (ret < 0) {
        return 0;
    }
    return result;
}