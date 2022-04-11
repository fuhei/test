/**
 * @file test_integer.c
 * @author Rong Tao <rongtao@cestc.cn>
 * @brief 整形加减乘除
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
init_integer_add(struct test_struct* test, void *arg)
{

}

static void _no_optimize
invoke_integer_add(struct test_struct* test, void *arg)
{
    int a, b, c, i;
    a = b = c = 1;

    for (i = 0; i < 1000000000; i++) {
        c = a + b;
    }
}

static void _no_optimize
release_integer_add(struct test_struct* test, void *arg)
{

}

struct test_struct test_integer_add = {
    .arg = NULL,
    .init_test = init_integer_add,
    .invoke_test = invoke_integer_add,
    .release_test = release_integer_add,
};


static void _no_optimize
init_integer_sub(struct test_struct* test, void *arg)
{

}

static void _no_optimize
invoke_integer_sub(struct test_struct* test, void *arg)
{
    int a, b, i;
    int _un_used c;
    a = 1000000000;
    b = 1;
    c = 0;

    for (i = 0; i < 1000000000; i++) {
        c = a - b;
    }
    //printf("sub\n");
}

static void _no_optimize
release_integer_sub(struct test_struct* test, void *arg)
{

}

struct test_struct test_integer_sub = {
    .arg = NULL,
    .init_test = init_integer_sub,
    .invoke_test = invoke_integer_sub,
    .release_test = release_integer_sub,
};


static void _no_optimize
init_integer_mul(struct test_struct* test, void *arg)
{

}

static void _no_optimize
invoke_integer_mul(struct test_struct* test, void *arg)
{
    int a, b, c, i;
    a = b = c = 1;

    for (i = 0; i < 1000000000; i++) {
        c = a * b;
    }
    //printf("mul\n");
}

static void _no_optimize
release_integer_mul(struct test_struct* test, void *arg)
{

}


struct test_struct test_integer_mul = {
    .arg = NULL,
    .init_test = init_integer_mul,
    .invoke_test = invoke_integer_mul,
    .release_test = release_integer_mul,
};


static void _no_optimize
init_integer_div(struct test_struct* test, void *arg)
{

}

static void _no_optimize
invoke_integer_div(struct test_struct* test, void *arg)
{
    int a, b, i;
    int _un_used c;

    a = 1000000000;
    b = 2;
    c = 1;

    for (i = 0; i < 1000000000; i++) {
        c = a / b;
    }
    //printf("div\n");
}

static void _no_optimize
release_integer_div(struct test_struct* test, void *arg)
{

}


struct test_struct test_integer_div = {
    .arg = NULL,
    .init_test = init_integer_div,
    .invoke_test = invoke_integer_div,
    .release_test = release_integer_div,
};