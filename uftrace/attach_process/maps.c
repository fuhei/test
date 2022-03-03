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

#include "maps.h"

int open_pid_maps(struct process *task)
{
    char proc_child_maps[] = "/proc/1234567890/maps";
	snprintf(proc_child_maps, sizeof(proc_child_maps), "/proc/%d/maps", task->pid);
	int mapsfd = open(proc_child_maps, O_RDONLY);
	if (mapsfd <= 0) {
		fprintf(stderr, "open %s failed.\n", proc_child_maps);
	}
    return mapsfd;
}

int open_pid_mem(struct process *task)
{
	char proc_child_mem[] = "/proc/1234567890/mem";
	snprintf(proc_child_mem, sizeof(proc_child_mem), "/proc/%d/mem", task->pid);
	int memfd = open(proc_child_mem, O_RDWR);
	if (memfd <= 0) {
		fprintf(stderr, "open %s failed.\n", proc_child_mem);
	}
    return memfd;
}


struct vma_struct * alloc_vma(struct process *task)
{
    struct vma_struct *vma = malloc(sizeof(struct vma_struct));
    assert(vma && "alloc vma failed.");
    memset(vma, 0x00, sizeof(struct vma_struct));

    list_init(&vma->node);

    return vma;
}


int free_vma(struct process *task, struct vma_struct * vma)
{
    if (!vma)
        return -1;

    free(vma);
    return 0;
}

static inline int __vma_rb_cmp(struct rb_node *node, unsigned long key) 
{
    struct vma_struct *vma = rb_entry(node, struct vma_struct, rb_node); 
    struct vma_struct *new = (struct vma_struct *)key;
    
    //TODO:
    if (new->end <= vma->start)
        return -1;
    else if (vma->start < new->end && vma->end > new->start)
        return 0;
    else if (vma->end <= new->start)
        return 1;
    
    printf("------------------------------------\n");
    print_vma(new);
    return 0;
}


int link_vma(struct process *task, struct vma_struct *vma)
{
    list_add(&vma->node, &task->vmas);
    rb_insert_node(&task->vmas_rb, &vma->rb_node, __vma_rb_cmp, (unsigned long)vma);

    return 0;
}

int unlink_vma(struct process *task, struct vma_struct *vma)
{
    list_del(&vma->node);
    rb_erase(&vma->rb_node, &task->vmas_rb);
    return 0;
}

void print_vma(struct vma_struct * vma)
{
    printf("%016lx-%016lx %6s %8lx %4x:%4x %8d %s\n",
            vma->start, vma->end, vma->perms, vma->offset,
            vma->maj, vma->min, vma->inode, vma->name_);
}