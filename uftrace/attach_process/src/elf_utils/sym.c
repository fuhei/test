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
	if (strlen(symname) > 1)
		ldebug("symname = %s\n", symname);
	print_elf_sym(sym);
}

void print_elf_all_syms(void *filemap, GElf_Ehdr *ehdr)
{
	elf_for_each_sym(filemap, ehdr, __print_sym, NULL);
}

void print_elf_sym(GElf_Sym *shm)
{
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