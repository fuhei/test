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
#include "file_utils.h"
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

struct insert_arg {
	struct process *task;
	unsigned long remote_addr;
	enum vma_type vma_type;
	struct vma_struct *insert_vma; /* return value */
};

static int __insert_vma_handler(struct vma_struct * tmpvma, void *arg)
{
	struct insert_arg *i_arg = (struct insert_arg*)arg;
	struct process *task = i_arg->task;
	struct vma_struct *find = find_vma(task, tmpvma->start);
	struct vma_struct *already_exist = find_vma(task, i_arg->remote_addr);

	if (already_exist) {
		test_maps_vma(task);
		lerror("%lx already in %d.%s.\n", i_arg->remote_addr, task->pid, already_exist->name_);
		return -1;
	}
	if (!find) {
		struct vma_struct *vma = alloc_vma();

		vma->start = tmpvma->start;
		vma->end = tmpvma->end;
		memcpy(vma->perms, tmpvma->perms, sizeof(vma->perms));
		vma->prot = __perms2prot(tmpvma->perms);
		vma->offset = tmpvma->offset;
		vma->maj = tmpvma->maj;
		vma->min = tmpvma->min;
		vma->inode = tmpvma->inode;
		strncpy(vma->name_, tmpvma->name_, sizeof(vma->name_));
		vma->type = i_arg->vma_type;

		link_vma(task, vma);

		/* Return */
		i_arg->insert_vma = vma;

		/* done for each */
		return -1;
	}
	return 0;
}

int remote_insert_object(struct process *task, char *objpath, enum vma_type type)
{
	int ret = 0;
	unsigned long remote_addr;
	ssize_t map_len = 0;
	int __unused remote_fd;
    struct stat statbuf;
	struct insert_arg handler_arg;
	struct vma_struct *obj_vma = NULL;

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

	handler_arg = (struct insert_arg) {
		.task = task,
		.remote_addr = remote_addr,
		.vma_type = type,
		.insert_vma = NULL,
	};

	/* Insert VMA */
	for_each_maps_vma(task, __insert_vma_handler, (void*)&handler_arg);

	/* mmap new object file into vma */
	if (handler_arg.insert_vma) {
		obj_vma = handler_arg.insert_vma;
		switch (type) {
		case VMA_FTRACE_TRAMPOLINE:
			task->ftrace_trampoline_obj = obj_vma;
			break;
		case VMA_PATCH_OBJ:
			//TODO: maybe is link list
		default:
			break;
		}

		mmap_file(objpath, (void **)&obj_vma->obj_local_mmap, &obj_vma->obj_size);
	} else {
		lerror("Insert object failed.\n");
	}

    return 0;

unmap_ret:
	remote_munmap(task, remote_addr, map_len);
close_ret:
	remote_close(task, remote_fd);
    return -1;
}