/**
 * @file test.c
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

#include "maps.h"
#include "log.h"

static void test_vma_list(struct process *task)
{
    struct vma_struct __unused *ivma = NULL;

	list_for_each_entry(ivma, &task->vmas, node) {
		print_vma(ivma);
	}
}
static void test_vma_rbtree(struct process *task)
{
    struct vma_struct __unused *ivma = NULL;
    ldebug("----- rbtree -------\n");
	struct rb_node *rnode;
	for (rnode = rb_first(&task->vmas_rb); rnode; rnode = rb_next(rnode)) {
		ivma = rb_entry(rnode, struct vma_struct, rb_node);
		print_vma(ivma);
	}
}
/**
 * @brief Test vma list
 *
 * @param task
 * @return int
 */
int test_maps_vma(struct process *task)
{
    test_vma_list(task);
    test_vma_rbtree(task);
    return 0;
}