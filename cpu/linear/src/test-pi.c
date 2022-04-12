/**
 * @file test-pi.c
 * @author Rong Tao <rongtao@cestc.cn>
 * @brief 计算 PI 值
 * @version 0.1
 * @date 2022-04-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


static double _un_used caculate_pi(int accuracy)
{
#if 0
    /**
     * @brief 摘自 https://stackoverflow.com/questions/9912151/math-constant-pi-value-in-c
     * 结果错误
     */
    int i;
    double result = 1;
    int a = 2;
    int b = 1;

    for (i = 0; i < accuracy; i ++) {
        result = a / b * result;
        if (a < b) {
            a = a + 2;
        } else if (b < a) {
            b = b + 2;
        }
    }

    return result * 2;
#else
    /**
     * @brief https://www.codeproject.com/Tips/5267987/Calculating-the-Pi-Number
     */
    double pi_4 = 0;
    int n = accuracy;
    int sign = 1;
    int i = 0;

    for (i = 1; i < n; i += 2) {
        if (sign) {
            pi_4 += 1.0 / i;
            sign = 0;
        } else {
            pi_4 -= 1.0 / i;
            sign = 1;
        }
    }
    return pi_4 * 4;
#endif
}

static void _no_optimize
init_pi(struct test_struct* test, void *arg)
{

}

static void _no_optimize
invoke_pi(struct test_struct* test, void *arg)
{
    double _un_used pi;

    while (--test->loop >= 0)
        pi = caculate_pi(TEST_PI_LOOP);
    //printf("pi = %lf\n", pi);
}

static void _no_optimize
release_pi(struct test_struct* test, void *arg)
{

}


struct test_struct test_pi = {
    .arg = NULL,
    .init_test = init_pi,
    .invoke_test = invoke_pi,
    .release_test = release_pi,
};