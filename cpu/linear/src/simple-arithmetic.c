/**
 * @file simple-arithmetic.c
 * @author Rong Tao <rongtao@cestc.cn>
 * @brief 简单算术计算
 * @version 0.1
 * @date 2022-04-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdlib.h>

#include "utils.h"


static void _no_optimize
init_arithmetic(struct test_struct* test, void *arg)
{

}

static void _no_optimize
invoke_arithmetic(struct test_struct* test, void *arg)
{
    unsigned long ul = 100;
    float f = 100;
    double d = 100;
    long double ld = 100;
    int loop = 1000000;

    while (loop--) {
        ld += d;
        d -= f;
        ul *= 2;
        ld /= 0.99;
    }
}

static void _no_optimize
release_arithmetic(struct test_struct* test, void *arg)
{

}


struct test_struct test_arithmetic = {
    .arg = NULL,
    .init_test = init_arithmetic,
    .invoke_test = invoke_arithmetic,
    .release_test = release_arithmetic,
};