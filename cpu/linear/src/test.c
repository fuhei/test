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


void test_execute(enum test_type type, int loop)
{
    struct test_struct *test = NULL;

    switch (type) {
    case TEST_ARITHMETIC:
        test = &test_arithmetic;
        break;
    case TEST_ARITHMETIC_INT_ADD:
        test = &test_integer_add;
        break;
    case TEST_ARITHMETIC_INT_SUB:
        test = &test_integer_sub;
        break;
    case TEST_ARITHMETIC_INT_MUL:
        test = &test_integer_mul;
        break;
    case TEST_ARITHMETIC_INT_DIV:
        test = &test_integer_div;
        break;
    case TEST_N_QUEEN:
        test = &test_N_queen;
        break;
    case TEST_PI:
        test = &test_pi;
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