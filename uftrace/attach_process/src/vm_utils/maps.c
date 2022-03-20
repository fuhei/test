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

		//print_vma(i_arg->insert_vma);

		/* done for each */
		return -1;
	}
	return 0;
}

int remote_insert_object(struct process *task, char *objreal, char *objcopy,
		unsigned long start, ssize_t size, enum vma_type type)
{
	int ret = 0;
	unsigned long remote_addr;
	ssize_t map_len = size;
	int __unused remote_fd;
	struct insert_arg handler_arg;
	struct vma_struct *obj_vma = NULL;

	remote_fd = remote_open(task, objcopy, O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (remote_fd <= 0) {
        lwarning("remote open failed. may pid %d don't have permission"\
					"write to '/tmp/'\n", task->pid);
        return -1;
    }

	ret = remote_ftruncate(task, remote_fd, map_len);
    if (ret != 0) {
        lwarning("remote ftruncate failed.\n");
        goto close_ret;
    }
	/**
	 * 是否需要mmap，mmap不是很好的选择
	 * MAP_PRIVATE: 不会同不到文件
	 * MAP_SHARED: 同步到文件
	 */
	remote_addr = remote_mmap(task,
				start, map_len,
				PROT_READ | PROT_WRITE | PROT_EXEC,
				MAP_SHARED, remote_fd, 0);
    if (remote_addr == (unsigned long)MAP_FAILED) {
        lerror("Remote map failed, %s\n", objcopy);
        goto unmap_ret;
    }

	remote_close(task, remote_fd);

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
		/* Ftrace */
		case VMA_FTRACE_TRAMPOLINE:
			task->ftrace_trampoline_obj = obj_vma;
			//print_vma(obj_vma);
			obj_vma->local_elf_file = malloc(sizeof(struct elf_file_struct));
			obj_vma->local_elf_file->belongs_to_vma = obj_vma;
			open_elf_file_and_copy(objreal, obj_vma->local_elf_file);

			/**
			 * Blacklist and sig check maybe here.
			 *
			 */

			break;
		/* Patch */
		case VMA_PATCH_OBJ:
			//TODO: maybe is link list
		default:
			break;
		}

	} else {
		lerror("Insert object failed.\n");
	}

	//test_maps_vma(task);

    return 0;

unmap_ret:
	remote_munmap(task, remote_addr, map_len);
close_ret:
	remote_close(task, remote_fd);
    return -1;
}