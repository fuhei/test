/**
 * @file trampoline.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-07
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
#include <libgen.h>
#include <getopt.h>

#include "util.h"
#include "log.h"
#include "elf_utils.h"
#include "file_utils.h"
#include "maps.h"
#include "process.h"
#include "syscalls.h"
#include "ftrace.h"
#include "patch.h"
#include "ftrace.h"

static int rewrite_section_headers(struct elf_file_struct *elf, unsigned long start, int flags)
{
	unsigned int i;

	/* This should always be true, but let's be sure. */
	elf->sechdrs[0].sh_addr = 0;

    for (i = 1; i < elf->ehdr->e_shnum; i++) {
		GElf_Shdr *shdr = &elf->sechdrs[i];
		if (shdr->sh_type != SHT_NOBITS
		    && elf->len < shdr->sh_offset + shdr->sh_size) {
			lerror("ELF len %lu truncated\n", elf->len);
			return -1;
		}
		shdr->sh_addr = (size_t)elf->ehdr + shdr->sh_offset;
        ldebug("Rewrite section addr: %lx\n", shdr->sh_addr);
	}
    return 0;
}

static int layout_and_allocate(struct elf_file_struct *elf, int flags)
{
    int i;

    if (!elf->belongs_to_vma) {
        lerror("No VMA belongs to.\n");
        return -1;
    }
    //TODO: maybe not needed

	for (i = 0; i < elf->ehdr->e_shnum; i++) {
		GElf_Shdr __unused *shdr = &elf->sechdrs[i];

        /* Address in target process address space */
        //shdr->sh_addr += elf->belongs_to_vma->start;
    }
    return 0;
}


static int simplify_symbols(struct process *task,
                            struct elf_file_struct *elf,
                            unsigned long target_start_addr)
{
	GElf_Shdr *symsec = &elf->sechdrs[elf->index.sym];
	GElf_Sym *sym = (void *)symsec->sh_addr;
	unsigned long secbase;
	unsigned int i;
	int __unused ret = 0;
    struct elf_file_sym_struct *elfsym;

	for (i = 1; i < symsec->sh_size / sizeof(GElf_Sym); i++) {
		char *name = elf->strtab + sym[i].st_name;

		switch (sym[i].st_shndx) {
		case SHN_COMMON:
            ldebug("Handle SHN_COMMON symbol %s\n", name);
			/* Ignore common symbols */
			if (!strncmp(name, "__gnu_lto", 9))
				break;

			/* We compiled with -fno-common.  These are not
			   supposed to happen.  */
			ldebug("Common symbol: %s\n", name);
			lwarning("please compile with -fno-common\n");
			ret = -1;
			break;
		case SHN_ABS:
			/* Don't need to do anything */
			ldebug("Absolute symbol: 0x%08lx\n", (long)sym[i].st_value);
			break;
        case SHN_UNDEF:
			lwarning("Handle SHN_UNDEF symbol %s\n", name);
            elfsym = elf_file_find_sym(task->libc_base->local_elf_file, name);
            lwarning("Find elfsym = %s, %lx\n", elfsym->symname,
                elfsym->sym->st_value + task->libc_base->start);
            sym[i].st_value = elfsym->sym->st_value + task->libc_base->start;
			break;
        default:
            secbase = elf->sechdrs[sym[i].st_shndx].sh_addr - (unsigned long)elf->ehdr + target_start_addr;
			sym[i].st_value += secbase;
            ldebug("Handle symbol %s(%s) %lx\n",
                name, elf_sh_name_string(symsec), sym[i].st_value);
            if (!strcmp(name, "_trampoline")) {
                task->ftrace_trampoline_obj->local_elf_file->trampoline_sym = &sym[i];
            }
            break;
        }
    }
    return 0;
}


static int apply_relocations(struct process *task, struct elf_file_struct *elf)
{
	unsigned int i;
	int err = 0;

    for (i = 1; i < elf->ehdr->e_shnum; i++) {
		unsigned int infosec = elf->sechdrs[i].sh_info;

		/* Not a valid relocation section? */
		if (infosec >= elf->ehdr->e_shnum)
			continue;

		/* Don't bother with non-allocated sections */
		if (!(elf->sechdrs[infosec].sh_flags & SHF_ALLOC))
			continue;

        if (elf->sechdrs[i].sh_type == SHT_RELA)
			err = apply_relocate_add(elf->sechdrs, elf->strtab,
						 elf->index.sym, i, task);
        else if (elf->sechdrs[i].sh_type == SHT_REL)
			err = apply_relocate(elf->sechdrs, elf->strtab,
					     elf->index.sym, i, task);
		if (err < 0)
			break;
    }
    return 0;
}

int ftrace_insert_trampoline_object(struct process *task, char *objpath)
{
    char objcopy[512] = {0};
    ssize_t size;
    unsigned long start = 0;

    file_size(objpath, &size);

    /**
     * CentOS-Stream-9 has permission write to /tmp/
     * however, Ubuntu doesn't.
     *
     */
    snprintf(objcopy, sizeof(objcopy), "/tmp/trampoline-%s", basename(objpath));
    //file_mktemp(objcopy);

    find_span_vma_area(task, size, &start);

    remote_insert_object(task, objpath, objcopy, start, size, VMA_FTRACE_TRAMPOLINE);

    rewrite_section_headers(task->ftrace_trampoline_obj->local_elf_file, start, 0);

    /* TODO: check version here */

    layout_and_allocate(task->ftrace_trampoline_obj->local_elf_file, 0);

    simplify_symbols(task, task->ftrace_trampoline_obj->local_elf_file, start);

#if 0
    ftrace_trampoline_object_relocate(task);
#else
    apply_relocations(task, task->ftrace_trampoline_obj->local_elf_file);
#endif
    memcpy_to_remote(task,
            task->ftrace_trampoline_obj->start,
            task->ftrace_trampoline_obj->local_elf_file->obj_local,
            task->ftrace_trampoline_obj->local_elf_file->obj_size);

    /* TODO: flush cache */

    return 0;
}