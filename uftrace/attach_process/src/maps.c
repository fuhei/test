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
#include "log.h"

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

struct vma_struct * alloc_vma(struct process *task)
{
    struct vma_struct *vma = malloc(sizeof(struct vma_struct));
    assert(vma && "alloc vma failed.");
    memset(vma, 0x00, sizeof(struct vma_struct));

    vma->type = VMA_NONE;

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
 * @brief Test vma list
 * 
 * @param task 
 * @return int 
 */
int test_maps_vma(struct process *task)
{
	struct vma_struct __unused *ivma = NULL;

	list_for_each_entry(ivma, &task->vmas, node) {
		print_vma(ivma);
	}

	list_for_each_entry(ivma, &task->vmas, node) {
		GElf_Ehdr *ehdr = (GElf_Ehdr*)ivma->content;
		if (check_elf_ehdr(ehdr) == -1) {
			continue;
		}
		linfo("%s is ELF.\n", ivma->name_);

		print_elf_ehdr(ehdr);

		print_elf_all_phdrs(ivma->content, ehdr);
		//print_elf_all_shdrs(buffer, ehdr);
	}
    
	ldebug("----- rbtree -------\n");
	struct rb_node *rnode;
	for (rnode = rb_first(&task->vmas_rb); rnode; rnode = rb_next(rnode)) {
		ivma = rb_entry(rnode, struct vma_struct, rb_node);
		print_vma(ivma);
	}
    return 0;
}