/**
 * @file ftrace.h
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include "compiler.h"
#include "process.h"

#define FTRACE_SECTION  ".ftrace.addon"


struct ftrace_metadata {

};



void test_ftrace(struct process *task);