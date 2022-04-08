/**
 * @file test-N-queen.c
 * @author Rong Tao <rongtao@cestc.cn>
 * @brief N 皇后问题计算
 * @version 0.1
 * @date 2022-04-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdlib.h>

#include "utils.h"


static void _no_optimize
init_N_queen(struct test_struct* test, void *arg)
{

}

static void _no_optimize
invoke_N_queen(struct test_struct* test, void *arg)
{

}

static void _no_optimize
release_N_queen(struct test_struct* test, void *arg)
{

}


struct test_struct test_N_queen = {
    .arg = NULL,
    .init_test = init_N_queen,
    .invoke_test = invoke_N_queen,
    .release_test = release_N_queen,
};