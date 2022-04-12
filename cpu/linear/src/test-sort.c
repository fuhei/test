/**
 * @file test-sort.c
 * @author Rong Tao <rongtao@cestc.cn>
 * @brief 计算 PI 值
 * @version 0.1
 * @date 2022-04-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <numa.h>

#include "utils.h"

static unsigned long nr_elements = 0;
static int *int_array = NULL;

void print_array()
{
    unsigned long i;
    for (i = 0; i < nr_elements; i++) {
        printf("%d ", int_array[i]);
    } printf("\n");
}

static void _no_optimize
init_qsort(struct test_struct* test, void *arg)
{
    unsigned long i;
    nr_elements = test->loop;
    printf("qsort nr_elements = %ld.\n", nr_elements);

    int_array = aligned_alloc(test->thread_arg->cacheline_size, sizeof(*int_array)*nr_elements);

    for (i = 0; i < nr_elements; i++) {
        /**
         * 初始化数组为： [0, 1, 2, 3, 4, ...]
         */
        int_array[i] = i;
    }
    //print_array();
}

static inline int int_cmp(const void *a, const void *b)
{
    int *A = (int *)a;
    int *B = (int *)b;
    return *B - *A;
}

static void _no_optimize
invoke_qsort(struct test_struct* test, void *arg)
{
    qsort(int_array, nr_elements, sizeof(int), int_cmp);
    printf("qsort.\n");
}

static void _no_optimize
release_qsort(struct test_struct* test, void *arg)
{
    //print_array();
}


struct test_struct test_qsort = {
    .arg = NULL,
    .init_test = init_qsort,
    .invoke_test = invoke_qsort,
    .release_test = release_qsort,
};