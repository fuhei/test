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

#define INST_SYSCALL    0x01, 0x00, 0x00, 0xd4  /*0xd4000001 svc #0  = syscall*/
#define INST_INT3       0xa0, 0x00, 0x20, 0xd4  /*0xd42000a0 brk #5  = int3*/
#define INST_CALLQ      modify_me               /* callq */
#define INST_JMPQ       modify_me               /* jmpq */

#define JMP_TABLE_JUMP_AARCH64  0xd61f022058000051 /*  ldr x17 #8; br x17 */
#define JMP_TABLE_JUMP_ARCH     JMP_TABLE_JUMP_AARCH64