/**
 * @file instruments.h
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#define INST_SYSCALL    0x0f, 0x05  /* syscall */
#define INST_INT3       0xcc        /* int3 */
#define INST_CALLQ      0xe8        /* callq */
#define INST_JMPQ       0xe9        /* jmpq */