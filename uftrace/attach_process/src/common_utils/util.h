/**
 * @file util.h
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdbool.h>

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE*)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
        ((type *)(((char *)(ptr)) - offsetof(type,member)))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))
#endif


struct args_opts {
    int verbose_flag;
    bool check_trace_func;
};