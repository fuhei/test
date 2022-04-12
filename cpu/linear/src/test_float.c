/**
 * @file test_float.c
 * @author Rong Tao <rongtao@cestc.cn>
 * @brief 浮点加减乘除
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


static void _no_optimize
init_float_add(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{

}

static void _no_optimize
invoke_float_add(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{
    int i;
    float a, b, c;
    a = b = c = 1;


    while (--thread_arg->test_loop >= 0)
    for (i = 0; i < TEST_ARITHMETIC_LOOP; i++) {
        c = a + b;
    }
    //printf("add\n");
}

static void _no_optimize
release_float_add(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{

}

struct test_struct test_float_add = {
    .arg = NULL,
    .init_test = init_float_add,
    .invoke_test = invoke_float_add,
    .release_test = release_float_add,
};


static void _no_optimize
init_float_sub(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{

}

static void _no_optimize
invoke_float_sub(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{
    int i;
    float a, b;
    float _un_used c;
    a = 1000000000;
    b = 1;
    c = 0;

    while (--thread_arg->test_loop >= 0)
    for (i = 0; i < TEST_ARITHMETIC_LOOP; i++) {
        c = a - b;
    }
    //printf("sub\n");
}

static void _no_optimize
release_float_sub(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{

}

struct test_struct test_float_sub = {
    .arg = NULL,
    .init_test = init_float_sub,
    .invoke_test = invoke_float_sub,
    .release_test = release_float_sub,
};


static void _no_optimize
init_float_mul(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{

}

static void _no_optimize
invoke_float_mul(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{
    int i;
    float a, b, c;
    a = b = c = 1;

    while (--thread_arg->test_loop >= 0)
    for (i = 0; i < TEST_ARITHMETIC_LOOP; i++) {
        c = a * b;
    }
    //printf("mul\n");
}

static void _no_optimize
release_float_mul(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{

}


struct test_struct test_float_mul = {
    .arg = NULL,
    .init_test = init_float_mul,
    .invoke_test = invoke_float_mul,
    .release_test = release_float_mul,
};


static void _no_optimize
init_float_div(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{

}

static void _no_optimize
invoke_float_div(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{
    int i;
    float a, b;
    float _un_used c;

    a = 1000000000;
    b = 2;
    c = 1;

    while (--thread_arg->test_loop >= 0)
    for (i = 0; i < TEST_ARITHMETIC_LOOP; i++) {
        c = a / b;
    }
    //printf("div\n");
}

static void _no_optimize
release_float_div(struct test_struct* test, struct pthread_args *thread_arg, void *arg)
{

}


struct test_struct test_float_div = {
    .arg = NULL,
    .init_test = init_float_div,
    .invoke_test = invoke_float_div,
    .release_test = release_float_div,
};