/**
 * @file maps.c
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
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#include "util.h"
#include "log.h"
#include "elf_utils.h"
#include "maps.h"
#include "process.h"
#include "syscalls.h"
#include "ftrace.h"
#include "patch.h"
#include "ftrace.h"

int open_pid_maps(struct process *task)
{
    char proc_child_maps[] = "/proc/1234567890/maps";
	snprintf(proc_child_maps, sizeof(proc_child_maps), "/proc/%d/maps", task->pid);
	int mapsfd = open(proc_child_maps, O_RDONLY);
	if (mapsfd <= 0) {
		lerror("open %s failed.\n", proc_child_maps);
	}
    return mapsfd;
}

int open_pid_mem(struct process *task)
{
	char proc_child_mem[] = "/proc/1234567890/mem";
	snprintf(proc_child_mem, sizeof(proc_child_mem), "/proc/%d/mem", task->pid);
	int memfd = open(proc_child_mem, O_RDWR);
	if (memfd <= 0) {
		lerror("open %s failed.\n", proc_child_mem);
	}
    return memfd;
}

int remote_insert_object(struct process *task, char *objpath)
{
	int ret = 0;
	unsigned long remote_addr;
	ssize_t map_len = 0;
	int __unused remote_fd;
    struct stat statbuf;

	remote_fd = remote_open(task, objpath, O_RDWR, 0644);
    if (remote_fd <= 0) {
        lwarning("remote open failed.\n");
        return -1;
    }
    remote_fstat(task, remote_fd, &statbuf);
    map_len = statbuf.st_size;
	ret = remote_ftruncate(task, remote_fd, map_len);
    if (ret != 0) {
        lwarning("remote ftruncate failed.\n");
        goto close_ret;
    }
	remote_addr = remote_mmap(task,
				0UL, map_len,
				PROT_READ | PROT_WRITE | PROT_EXEC,
				MAP_PRIVATE, remote_fd, 0);
    if (remote_addr == (unsigned long)MAP_FAILED) {
        lerror("Remote map failed, %s\n", objpath);
        goto unmap_ret;
    }
    return 0;

unmap_ret:
	remote_munmap(task, remote_addr, map_len);
close_ret:
	remote_close(task, remote_fd);
    return -1;
}