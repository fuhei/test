/**
 * @file utils.h
 * @author Rong Tao <rongtao@cestc.cn>
 * @brief Tools
 * @version 0.1
 * @date 2022-04-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <numa.h>
#include <stdbool.h>
#include <pthread.h>

enum test_type {
    TEST_ARITHMETIC,    //简单算术运算，混合运算
    TEST_ARITHMETIC_INT_ADD,    //整形加法
    TEST_ARITHMETIC_INT_SUB,    //整形减法
    TEST_ARITHMETIC_INT_MUL,    //整形乘法
    TEST_ARITHMETIC_INT_DIV,    //整形除法
    TEST_ARITHMETIC_FLOAT_ADD,    //整形加法
    TEST_ARITHMETIC_FLOAT_SUB,    //整形减法
    TEST_ARITHMETIC_FLOAT_MUL,    //整形乘法
    TEST_ARITHMETIC_FLOAT_DIV,    //整形除法
    TEST_N_QUEEN,       // N 皇后
    TEST_PI,            // PI 计算
    TEST_SORT_QSORT,    // 快速排序
    TEST_NUM,
};

#define TEST_LOOP 1

struct pthread_args {
    int cpu;
    int numaid;
    int cacheline_size;
    enum test_type test_type; // 和 struct args_opt.test_type 值相同
    unsigned long test_loop; // 和 struct args_opt.test_loop 值相同
    struct timespec start, end;
};

#define _un_used __attribute__((unused))
#define _no_optimize __attribute__((optimize("-O0")))

#define TEST_ARITHMETIC_LOOP    1000000000
#define TEST_PI_LOOP    10000000
#define TEST_QSORT_NR_ELEMENTS    100000

struct test_struct {
    void *arg;
    int loop;   // thread_arg.test_loop
    struct pthread_args *thread_arg;

    /* 定义函数时，清添加 _no_optimize 属性 */
    void (*init_test)(struct test_struct*, void *arg);
    void (*invoke_test)(struct test_struct*, void *arg);
    void (*release_test)(struct test_struct*, void *arg);
};


struct args_opt {
    int run_on_numa;
    int min_cpu;
    int max_cpu;
    int interval_cpu;
    int nr_thread; // 用 max_cpu, min_cpu, interval_cpu 计算得到
    pthread_t *threadids;
    struct pthread_args **threads_args;
    int cacheline_size;
    enum test_type test_type;
    bool custom_test_loop;
    unsigned long test_loop;
};


struct numa_node_info {
    int numaid;
    struct bitmask cpumask;
};

struct cache_info {
    struct {
        int assoc;
        int size;
        int linesize;
    } l1d, l1i, l2, l3, l4, *llc;
};


struct system_info {
    int nr_numa_node;
    int num_possible_cpus;
    struct numa_node_info *numainfos;
    /* Cache */
    int nr_cache_level;
    struct cache_info cache_info;
};


extern struct test_struct test_integer_add;
extern struct test_struct test_integer_sub;
extern struct test_struct test_integer_mul;
extern struct test_struct test_integer_div;
extern struct test_struct test_float_add;
extern struct test_struct test_float_sub;
extern struct test_struct test_float_mul;
extern struct test_struct test_float_div;
extern struct test_struct test_arithmetic;
extern struct test_struct test_N_queen;
extern struct test_struct test_pi;
extern struct test_struct test_qsort;

/**
 * @brief 执行一个测试
 *
 * @param type  测试类型，见 enum test_type
 * @param loop  执行 invoke_test() 的次数
 */
void test_execute(struct pthread_args *thread_arg);