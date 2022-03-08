/**
 * @file sym.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-07
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
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>

#include "util.h"
#include "log.h"
#include "maps.h"
#include "elf_utils.h"
#include "compiler.h"


int elf_for_each_sym(void *filemap, GElf_Ehdr *ehdr,
        void (*handler)(GElf_Shdr *shdr, GElf_Sym *symtab, GElf_Sym *sym, char *symname, void *arg), void *arg)
{
	GElf_Shdr *shdr;
	GElf_Sym *sym;
	int i, symidx = -1;
	char *strsym;

    shdr = (void *)filemap + ehdr->e_shoff;
	for (i = 1; i < ehdr->e_shnum; i++) {
		GElf_Shdr *s = shdr + i;
		if (s->sh_type == SHT_SYMTAB) {
			symidx = i;
            ldebug("SHT_SYMTAB, symidx = %d, section: %s\n",
                symidx, elf_secname(filemap, s));
        }
    }

	if (symidx == ehdr->e_shnum) {
		lerror("No symtab in ELF.\n");
		return -1;
	}

    sym = (void *)filemap + shdr[symidx].sh_offset;
	strsym = (void *)filemap + shdr[shdr[symidx].sh_link].sh_offset;
	for (i = 1; i < shdr[symidx].sh_size / sizeof(GElf_Sym); i++) {
		GElf_Sym *isym = sym + i;
		char *symname = strsym + isym->st_name;
        
        handler(shdr, sym, isym, symname, arg);
	}

	return 0;
}

static void __print_sym(GElf_Shdr *shdr, GElf_Sym *symtab, GElf_Sym *sym, char *symname, void *arg)
{
	if (strlen(symname) > 1) {
		ldebug("symname = %s\n", symname);
		print_elf_sym(sym);
	}
}

void print_elf_all_syms(void *filemap, GElf_Ehdr *ehdr)
{
	elf_for_each_sym(filemap, ehdr, __print_sym, NULL);
}

void print_elf_sym(GElf_Sym *sym)
{
	ldebug("	%8ld %4d %4d %4d %16lx %16ld\n",
			sym->st_name, sym->st_info, sym->st_other, sym->st_shndx, sym->st_value, sym->st_size);
	//TODO
}


int elf_sym_count_undefined(void *filemap, GElf_Ehdr *ehdr)
{
	GElf_Shdr *shdr;
	GElf_Sym *sym;
	int i, symidx = 0, count = 0;
	char *strsym;

    shdr = (void *)ehdr + ehdr->e_shoff;
    for (i = 1; i < ehdr->e_shnum; i++) {
        GElf_Shdr *s = shdr + i;
        if (s->sh_type == SHT_SYMTAB) {
            symidx = i;
            break;
        }
    }
    sym = (void *)ehdr + shdr[symidx].sh_offset;
	strsym = (void *)ehdr + shdr[shdr[symidx].sh_link].sh_offset;
	for (i = 1; i < shdr[symidx].sh_size / sizeof(GElf_Sym); i++) {
		GElf_Sym *s = sym + i;

		if (s->st_shndx == SHN_UNDEF &&
		    GELF_ST_BIND(s->st_info) == STB_GLOBAL) {
			count++;
			ldebug("Undefined symbol '%s'\n", strsym + s->st_name);
		}
	}

	return count;
}

static int __sym_struct_rb_insert_cmp_whole(struct rb_node *node, unsigned long key)
{
    struct elf_file_sym_struct *sym_struct = rb_entry(node, struct elf_file_sym_struct, sym_node); 
    struct elf_file_sym_struct *new = (struct elf_file_sym_struct *)key;

	return strcmp(sym_struct->symname, new->symname);
}

static int __sym_struct_rb_insert_cmp_symname(struct rb_node *node, unsigned long key)
{
    struct elf_file_sym_struct *sym_struct = rb_entry(node, struct elf_file_sym_struct, sym_node); 
    char *symname = (char *)key;

	return strcmp(sym_struct->symname, symname);
}

static void __insert_sym_to_rbtree(GElf_Shdr *shdr, GElf_Sym *symtab, GElf_Sym *sym, char *symname, void *arg)
{
	struct process *task = (struct process *)arg;
	struct elf_file_sym_struct *sym_struct;

	/* Is enough? */
	if (strlen(symname) <= 1)
		return;

	task->elf_file.symtab_shdr = shdr;
	task->elf_file.symtab = symtab;
	
	sym_struct = malloc(sizeof(struct elf_file_sym_struct));
	if (!sym_struct) {
		lerror("Alloc sym_struct failed\n");
		exit(1);
	}

	/* Fill */
	sym_struct->sym = sym;
	sym_struct->symname = symname;

	rb_insert_node(&task->elf_file.syms_rb, &sym_struct->sym_node,
		__sym_struct_rb_insert_cmp_whole, (unsigned long)sym_struct);
}

int elf_file_sym_rbtree_init(struct process *task)
{
	/* Rbtree of syms in COMM ELF */
    rb_init(&task->elf_file.syms_rb);
	elf_for_each_sym(task->elf_file.file_map, task->elf_file.ehdr,
		__insert_sym_to_rbtree, task);

	return 0;
}

struct elf_file_sym_struct *
elf_file_find_sym(struct process *task, char *symname)
{
	struct rb_node * rnode = rb_search_node(&task->elf_file.syms_rb,
                                __sym_struct_rb_insert_cmp_symname,
                                (unsigned long)symname);
    if (rnode) {
        return rb_entry(rnode, struct elf_file_sym_struct, sym_node);
    }
    return NULL;
}