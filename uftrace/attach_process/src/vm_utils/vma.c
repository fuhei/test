/**
 * @file vma.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-07
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
#include <sys/mman.h>

#include "maps.h"
#include "log.h"


struct vma_struct * alloc_vma()
{
    struct vma_struct *vma = malloc(sizeof(struct vma_struct));
    assert(vma && "alloc vma failed.");
    memset(vma, 0x00, sizeof(struct vma_struct));

    vma->type = VMA_NONE;

    list_init(&vma->node);

    return vma;
}

int free_vma(struct vma_struct * vma)
{
    if (!vma)
        return -1;

    free(vma);
    return 0;
}

/**
 * @brief For each VMA in /proc/PID/maps
 *
 * @param task
 * @param handler if return -1, 'for each' will terminate.
 * @param arg
 * @return int
 */
int for_each_maps_vma(struct process *task, int (*handler)(struct vma_struct * tmpvma, void *arg), void *arg)
{
    int handler_ret = 0;
    struct vma_struct vma;

    fseek(task->mapsfp, 0, SEEK_SET);
    do {
		unsigned long start, end, offset;
		unsigned int maj, min, inode;
		char perms[5], name_[256];
		int r;
		char line[1024];

		if (!fgets(line, sizeof(line), task->mapsfp))
			break;

		r = sscanf(line, "%lx-%lx %s %lx %x:%x %d %255s",
			   &start, &end, perms, &offset,
			   &maj, &min, &inode, name_);
		if (r <= 0) {
			lerror("sscanf failed.\n");
            return -1;
		}

		vma.start = start;
		vma.end = end;
		memcpy(vma.perms, perms, sizeof(vma.perms));
		vma.prot = __perms2prot(perms);
		vma.offset = offset;
		vma.maj = maj;
		vma.min = min;
		vma.inode = inode;
		strncpy(vma.name_, name_, sizeof(vma.name_));

        handler_ret = handler(&vma, arg);
        if (handler_ret == -1) {
            break;
        }
    } while (1);

    return 0;
}

int for_each_process_vma(struct process *task, int (*handler)(struct vma_struct * vma, void *arg), void *arg)
{
    struct vma_struct __unused *ivma = NULL;

	list_for_each_entry(ivma, &task->vmas, node) {
		handler(ivma, arg);
	}
    return 0;
}


unsigned int __perms2prot(char *perms)
{
    unsigned int prot = 0;

	if (perms[0] == 'r')
		prot |= PROT_READ;
	if (perms[1] == 'w')
		prot |= PROT_WRITE;
	if (perms[2] == 'x')
		prot |= PROT_EXEC;
	/* Ignore 'p'/'s' flag, we don't need it */
	return prot;
}

static inline int __vma_rb_insert_cmp(struct rb_node *node, unsigned long key)
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

    assert(0 && "Try insert illegal vma.");
    return 0;
}

int link_vma(struct process *task, struct vma_struct *vma)
{
    vma->task = task;
    list_add(&vma->node, &task->vmas);
    rb_insert_node(&task->vmas_rb, &vma->rb_node, __vma_rb_insert_cmp, (unsigned long)vma);

    return 0;
}

int unlink_vma(struct process *task, struct vma_struct *vma)
{
    vma->task = NULL;
    list_del(&vma->node);
    rb_erase(&vma->rb_node, &task->vmas_rb);
    return 0;
}

static inline int __find_vma_cmp(struct rb_node *node, unsigned long vaddr)
{
    struct vma_struct *vma = rb_entry(node, struct vma_struct, rb_node);

    if (vma->start > vaddr)
        return -1;
    else if (vma->start <= vaddr && vma->end > vaddr)
        return 0;
    else
        return 1;
}

struct vma_struct *find_vma(struct process *task, unsigned long vaddr)
{
    struct rb_node * rnode = rb_search_node(&task->vmas_rb,
                                __find_vma_cmp,
                                vaddr);
    if (rnode) {
        return rb_entry(rnode, struct vma_struct, rb_node);
    }
    return NULL;
}

void print_vma(struct vma_struct * vma)
{
    if (!vma) {
        lerror("Invalide pointer.\n");
        return ;
    }
    printf("%10s: %016lx-%016lx %6s %8lx %4x:%4x %8d %s\n",
            VMA_TYPE_NAME(vma->type),
            vma->start, vma->end, vma->perms, vma->offset,
            vma->maj, vma->min, vma->inode, vma->name_);
}

/**
 * @brief Find a span VMA start address
 *
 * @param task
 * @param size
 * @param start
 * @return int
 */
int find_span_vma_area(struct process *task, ssize_t size, unsigned long *start)
{
    struct vma_struct *ivma;
    struct rb_node * rnode;

	for (rnode = rb_first(&task->vmas_rb); rnode; rnode = rb_next(rnode)) {
		ivma = rb_entry(rnode, struct vma_struct, rb_node);
        struct rb_node *next_node = rb_next(rnode);
        struct vma_struct *next_vma;
        if (!next_node) {
            return -1;
        }
        next_vma = rb_entry(next_node, struct vma_struct, rb_node);
        if (next_vma->start - ivma->end >= size) {
            *start = ivma->end;
            return 0;
        }
	}
    lerror("No space fatal in target process, pid %d\n", task->pid);
    return -1;
}