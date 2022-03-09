/**
 * @file reloc.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "util.h"
#include "log.h"
#include "elf_utils.h"
#include "maps.h"
#include "compiler.h"
#include "process.h"

/**
 * .rela.text
 * .rela.eh_frame
 * [...]
 */
static int __apply_rela(char *elffile, GElf_Shdr *relsec)
{
	GElf_Ehdr __unused *ehdr = (void *)elffile;
	GElf_Shdr __unused *shdr = (void *)ehdr + ehdr->e_shoff;
    GElf_Shdr __unused *symhdr;

    /* Root of rela(s) */
    GElf_Rela __unused *relocs = (void *)ehdr + relsec->sh_offset;

    /* Additional section information for relsec */
    GElf_Shdr *__unused tshdr = shdr + relsec->sh_info;

    /*  */
    void __unused *t = (void *)ehdr + tshdr->sh_offset;
	void __unused *tshdr2 = (void *)tshdr->sh_addr;
	int i;

    char *secname;
    char *strsym;

    for (i = 1; i < ehdr->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_SYMTAB)
            symhdr = &shdr[i];
    }
    secname = elf_secname(ehdr, tshdr);
    strsym = (void *)elffile + shdr[symhdr->sh_link].sh_offset;

    ldebug("RELA. <.rela%s>\n", secname);

    /* For each rela in this section */
	for (i = 0; i < relsec->sh_size / sizeof(*relocs); i++) {
		GElf_Rela *rela = relocs + i;
		GElf_Sym *sym = (GElf_Sym *)((void *)ehdr + symhdr->sh_offset) + GELF_R_SYM(rela->r_info);
        char *symname = strsym + sym->st_name;
        unsigned long val;
		void *loc_local, *relative_loc_remote;

        /* Location in our address space */
		loc_local = t + rela->r_offset;
		/* Location in target process address space (for relative addressing) */
		relative_loc_remote = tshdr2 + rela->r_offset;

        val = sym->st_value + rela->r_addend;

        ldebug("%30s: %16lx - %16lx: %16lx (%lx)\n", symname, loc_local, relative_loc_remote, val, elffile);
    }
    return 0;
}

int ftrace_trampoline_object_relocate(struct process *task)
{
	GElf_Ehdr *ehdr;
	GElf_Shdr *shdr, __unused *symhdr;
    int i;

    if (!task->ftrace_trampoline_obj) {
        lerror("Not load trampoline object yet.\n");
        return -1;
    }

	ehdr = (void *)task->ftrace_trampoline_obj->obj_local;
	shdr = (void *)ehdr + ehdr->e_shoff;

    if (check_elf_ehdr(ehdr)) {
        lerror("Not ELF.\n", task->ftrace_trampoline_obj->name_);
        return -1;
    }
    for (i = 1; i < ehdr->e_shnum; i++) {
        GElf_Shdr *s = shdr + i;

        /* Not a valid relocation section? */
        if (s->sh_info >= ehdr->e_shnum)
            continue;

        if (s->sh_type == SHT_RELA) {
            __apply_rela((void*)ehdr, s);
        } else if (shdr->sh_type == SHT_REL) {
            //TODO
        }
    }

    return 0;
}