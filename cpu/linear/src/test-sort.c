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

struct qsort_test_data {
    unsigned long nr_elements;
    int *int_array;
};

void print_array(struct qsort_test_data *data)
{
    unsigned long i;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < data->nr_elements; i++) {
        printf("%d ", data->int_array[i]);
    } printf("\n");
    pthread_mutex_unlock(&mutex);
}

static void _no_optimize
init_qsort(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{
    unsigned long i;
    struct qsort_test_data *data;

    data = malloc(sizeof(struct qsort_test_data));
    thread_arg->test_data = data;

    data->nr_elements = thread_arg->test_loop;
    //printf("qsort nr_elements = %ld.\n", data->nr_elements);

    data->int_array = aligned_alloc(thread_arg->cacheline_size,
                        sizeof(int)*data->nr_elements);

    for (i = 0; i < data->nr_elements; i++) {
        /**
         * 初始化数组为： [0, 1, 2, 3, 4, ...]
         */
        data->int_array[i] = i;
    }
    //print_array(data);
}

static inline int int_cmp(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}

static void _no_optimize
invoke_qsort(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{
    struct qsort_test_data *data = thread_arg->test_data;

    qsort(data->int_array, data->nr_elements, sizeof(int), int_cmp);
    //printf("qsort.\n");
}

static void _no_optimize
release_qsort(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{
    struct qsort_test_data *data = thread_arg->test_data;
    //print_array(data);
    free(data->int_array);
    free(data);
}


struct test_struct test_qsort = {
    .arg = NULL,
    .init_test = init_qsort,
    .invoke_test = invoke_qsort,
    .release_test = release_qsort,
};