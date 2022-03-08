/**
 * @file test.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>

#include "util.h"
#include "log.h"
#include "elf_utils.h"
#include "maps.h"
#include "syscalls.h"
#include "patch.h"

static void test_single_ftrace(struct process *task)
{
    /* 0xe9： jmp in x86 */
	unsigned char orginal_code[5];
	unsigned char code[] = {0xe8, 0xd8, 0xff, 0xff, 0xff};
    struct vma_struct *func_vma, *trampoline_vma;
    struct elf_file_sym_struct *func_sym, *_trampoline_sym;

	func_sym = elf_file_find_sym(task, "func2");
	_trampoline_sym = elf_file_find_sym(task, "my_tramp");

	if (!_trampoline_sym || !func_sym) {
		lerror("No find func_sym or _trampoline_sym\n");
		exit(1);
	}
	lwarning("func_sym: %16lx\n", func_sym->sym->st_value);
	lwarning("_trampoline_sym: %16lx\n", _trampoline_sym->sym->st_value);

	//
	*(unsigned int *)(code + 1) = (unsigned int)(_trampoline_sym->sym->st_value - (func_sym->sym->st_value) - 9);

	func_vma = find_vma(task, func_sym->sym->st_value);
	ldebug("func_vma = %s, %ld\n", func_vma->name_, sizeof(_trampoline_sym->sym->st_value));
	trampoline_vma = find_vma(task, _trampoline_sym->sym->st_value);
	ldebug("trampoline_vma = %s\n", trampoline_vma->name_);

    /* Get original code */
	memcpy_from_remote(task, &orginal_code, func_sym->sym->st_value + 4, sizeof(orginal_code));

    /* Set jmp function */
	memcpy_to_remote(task, func_sym->sym->st_value + 4, &code, sizeof(code));

	process_detach(task);

    /* Make target process running */
	sleep(5);

    process_attach(task);

    /* Recover */
	memcpy_to_remote(task, func_sym->sym->st_value + 4, &orginal_code, sizeof(code));

    return;
}

void test_ftrace(struct process *task)
{
    test_single_ftrace(task);
}