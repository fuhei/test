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


void test_execute(struct pthread_args *thread_arg)
{
    struct test_struct *test = NULL;

    switch (thread_arg->test_type) {
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
    case TEST_ARITHMETIC_FLOAT_ADD:
        test = &test_float_add;
        break;
    case TEST_ARITHMETIC_FLOAT_SUB:
        test = &test_float_sub;
        break;
    case TEST_ARITHMETIC_FLOAT_MUL:
        test = &test_float_mul;
        break;
    case TEST_ARITHMETIC_FLOAT_DIV:
        test = &test_float_div;
        break;
    case TEST_N_QUEEN:
        test = &test_N_queen;
        break;
    case TEST_PI:
        test = &test_pi;
        break;
    case TEST_SORT_QSORT:
        test = &test_qsort;
        break;
    default:
        fprintf(stderr, "No test type %d, check -t, --type argument.\n",
                thread_arg->test_type);
        return;
    }

    thread_arg->test = test;

    if (test->init_test) {
        clock_gettime(CLOCK_MONOTONIC, &thread_arg->start_init);
        test->init_test(test, thread_arg, test->arg);
        clock_gettime(CLOCK_MONOTONIC, &thread_arg->end_init);
    }
    if (test->invoke_test) {
        clock_gettime(CLOCK_MONOTONIC, &thread_arg->start_invoke);
        test->invoke_test(test, thread_arg, test->arg);
        clock_gettime(CLOCK_MONOTONIC, &thread_arg->end_invoke);
    }
    if (test->release_test) {
        clock_gettime(CLOCK_MONOTONIC, &thread_arg->start_release);
        test->release_test(test, thread_arg, test->arg);
        clock_gettime(CLOCK_MONOTONIC, &thread_arg->end_release);
    }
}