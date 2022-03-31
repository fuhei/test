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
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "log.h"
#include "process.h"
#include "elf_utils.h"
#include "file_utils.h"
#include "compiler.h"


static int __sym_struct_rb_insert_cmp_whole(struct rb_node *node, unsigned long key)
{
    struct elf_file_sym_struct *sym_struct = rb_entry(node, struct elf_file_sym_struct, sym_node);
    struct elf_file_sym_struct *new = (struct elf_file_sym_struct *)key;

	return strcmp(sym_struct->symname, new->symname);
}


static void __insert_sym_to_rbtree(GElf_Shdr *shdr, GElf_Sym *symtab, GElf_Sym *sym, char *symname, void *arg)
{
	struct elf_file_struct *elf_file = (struct elf_file_struct *)arg;
	struct elf_file_sym_struct *sym_struct;

	/* Is enough? */
	if (strlen(symname) <= 1)
		return;

	sym_struct = malloc(sizeof(struct elf_file_sym_struct));
	if (!sym_struct) {
		lerror("Alloc sym_struct failed\n");
		exit(1);
	}

	/* Fill */
	sym_struct->sym = sym;
	sym_struct->symname = symname;

	rb_insert_node(&elf_file->syms_rb, &sym_struct->sym_node,
		__sym_struct_rb_insert_cmp_whole, (unsigned long)sym_struct);
}


static int open_elf_file_internal(char *file, struct elf_file_struct *elf, bool newmalloc)
{
    int ret, i;

	/* Mmap absolute path of comm, like /usr/bin/ls */
	if (newmalloc) {
		ret = mmap_file_and_copy(file, (void*)&elf->obj_local, &elf->obj_size);
	} else {
		ret = mmap_file_rdonly(file, &elf->file_map, &elf->file_len);
	}
    if (ret != 0) {
        lerror("Fail mmap file %s\n", file);
        return -1;
    }
	if (check_elf_ehdr_magic(elf->ehdr) != 0) {
		munmap_file((void*)elf->ehdr, elf->file_len);
		elf->ehdr = NULL;
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
		switch (elf->sechdrs[i].sh_type) {
		case SHT_SYMTAB:
			elf->index.sym = i;
			elf->index.str = elf->sechdrs[i].sh_link;
			elf->strtab = (char *)elf->ehdr
				+ elf->sechdrs[elf->index.str].sh_offset;
			break;
		case SHT_DYNSYM:
			elf->index.dynsym = i;
			elf->index.dynstr = elf->sechdrs[i].sh_link;
			elf->dynstrtab  = (char *)elf->ehdr
				+ elf->sechdrs[elf->index.dynstr].sh_offset;
			break;
		default:
			break;
		}
	}
	if (elf->index.sym == 0) {
		lerror("%s: object has no symbols (stripped?)\n",
			file ?: "(missing .objinfo section or name field)");
		return -1;
	}

	/* Rbtree of syms in COMM ELF */
    rb_init(&elf->syms_rb);
	elf_for_each_sym(elf, __insert_sym_to_rbtree, elf);

    return 0;
}

int open_elf_file(char *file, struct elf_file_struct *elf)
{
	return open_elf_file_internal(file, elf, false);
}

int open_elf_file_and_copy(char *file, struct elf_file_struct *elf)
{
	return open_elf_file_internal(file, elf, true);
}

/* Find a section: 0 means not found. */
unsigned int elf_find_sec(const struct elf_file_struct *elf, const char *name)
{
	unsigned int i;
	for (i = 1; i < elf->ehdr->e_shnum; i++) {
		GElf_Shdr *shdr = &elf->sechdrs[i];
		/* Alloc bit cleared means "ignore it." */
		if ((shdr->sh_flags & SHF_ALLOC)
		    && strcmp(elf->secstrings + shdr->sh_name, name) == 0)
			return i;
	}
	return 0;
}