/**
 * @file test.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-03-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "log.h"
#include "maps.h"
#include "elf_utils.h"
#include "compiler.h"



int test_elf(struct process *task)
{
	struct vma_struct __unused *ivma = NULL;

	list_for_each_entry(ivma, &task->vmas, node) {
		GElf_Ehdr *ehdr = (GElf_Ehdr*)ivma->content;
        if (!ehdr)
            continue;
		if (check_elf_ehdr(ehdr) == -1) {
			continue;
		}
        if (ivma->type != VMA_SELF) {
            continue;
        }

		linfo("%s is ELF.\n", ivma->name_);

		//print_elf_ehdr(ehdr);

		//print_elf_all_phdrs(ivma->content, ehdr);
		print_elf_all_shdrs(ivma->content, ehdr);
	}
    
    return 0;
}