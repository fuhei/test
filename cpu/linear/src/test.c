/**
 * @file test.c
 * @author Rong Tao <rongtao@cestc.cn>
 * @brief 执行测试例
 * @version 0.1
 * @date 2022-04-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

extern struct test_struct test_arithmetic;

void test_execute(enum test_type type, int loop)
{
    struct test_struct *test = NULL;

    switch (type) {
    case TEST_ARITHMETIC:
        test = &test_arithmetic;
        break;
    default:
        fprintf(stderr, "No test type %d, check -t, --type argument.\n", type);
        return;
    }

    if (test->init_test) {
        test->init_test(test, test->arg);
    }
    if (test->invoke_test) {
        while (--loop >= 0) {
            test->invoke_test(test, test->arg);
        }
    }
    if (test->release_test) {
        test->release_test(test, test->arg);
    }
}