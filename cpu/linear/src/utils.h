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

enum test_type {
    TEST_ARITHMETIC, //简单算术运算
    TEST_NUM,
};

#define _no_optimize __attribute__((optimize("-O0")))

struct test_struct {
    void *arg;
    /* 定义函数时，清添加 _no_optimize 属性 */
    void (*init_test)(struct test_struct*, void *arg);
    void (*invoke_test)(struct test_struct*, void *arg);
    void (*release_test)(struct test_struct*, void *arg);
};

/**
 * @brief 执行一个测试
 *
 * @param type  测试类型，见 enum test_type
 * @param loop  执行 invoke_test() 的次数
 */
void test_execute(enum test_type type, int loop);