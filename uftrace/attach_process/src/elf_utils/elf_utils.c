/**
 * @file elf_utils.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-03
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
#include "elf_utils.h"
#include "file_utils.h"
#include "compiler.h"

int open_elf_file(char *file, struct elf_file_struct *elf)
{
    int ret, i;

	/* Mmap absolute path of comm, like /usr/bin/ls */
    ret = mmap_file_rdonly(file, &elf->file_map, &elf->file_len);
    if (ret != 0) {
        lerror("Fail mmap file %s\n", file);
        return -1;
    }
	if (check_elf_ehdr_magic(elf->file_map) != 0) {
		munmap_file(elf->file_map, elf->file_len);
		elf->file_map = NULL;
		elf->file_len = 0;
		return -1;
	}

	/* Get section header */
	elf->sechdrs = (void *)elf->ehdr
				+ elf->ehdr->e_shoff;
	elf->secstrings = (void *)elf->ehdr
		+ elf->sechdrs[elf->ehdr->e_shstrndx].sh_offset;

	/* Find internal symbols and strings. */
	for (i = 1; i < elf->ehdr->e_shnum; i++) {
		if (elf->sechdrs[i].sh_type == SHT_SYMTAB) {
			elf->index.sym = i;
			elf->index.str = elf->sechdrs[i].sh_link;
			elf->strtab = (char *)elf->ehdr
				+ elf->sechdrs[elf->index.str].sh_offset;
			break;
		}
	}
	if (elf->index.sym == 0) {
		lerror("%s: object has no symbols (stripped?)\n",
			file ?: "(missing .objinfo section or name field)");
		return -1;
	}

    return 0;
}