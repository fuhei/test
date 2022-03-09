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

#define FTRACE_TEXT ".uftrace.text"
#define FTRACE_DATA ".uftrace.data"

#define __ftrace_text __section(FTRACE_TEXT)
#define __ftrace_data __section(FTRACE_DATA)

//TODO

void test_ftrace(struct process *task);


int ftrace_insert_trampoline_object(struct process *task, char *objpath);
int ftrace_trampoline_object_relocate(struct process *task);