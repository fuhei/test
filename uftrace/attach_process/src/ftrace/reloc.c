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
#include <errno.h>

#include "util.h"
#include "log.h"
#include "elf_utils.h"
#include "maps.h"
#include "compiler.h"
#include "process.h"

#if defined(__x86_64__)
#include "arch/x86_64/instruments.h"
#elif defined(__aarch64__)
#include "arch/aarch64/instruments.h"
#endif


int
arch_x86_64_apply_relocate(struct process *task, GElf_Rela *r, GElf_Sym *s,
							void *loc, void *loc2, unsigned long val)
{
	switch (GELF_R_TYPE(r->r_info)) {
	case R_X86_64_NONE:
		break;
	case R_X86_64_64:
#if 0
		if (*(uint64_t*)loc != 0)
			goto invalid_relocation;
#endif
		*(unsigned long *)loc = val;
		break;
	case R_X86_64_32:
#if 0
		if (*(uint32_t*)loc != 0)
			goto invalid_relocation;
#endif
		*(unsigned int *)loc = val;
        if (val != *(uint32_t *)loc)
            goto overflow;
		break;
	case R_X86_64_32S:
#if 0
		if (*(int32_t*)loc != 0)
			goto invalid_relocation;
#endif
		*(signed int *)loc = val;
        if ((int64_t)val != *(int32_t *)loc)
            goto overflow;
		break;
	case R_X86_64_GOTTPOFF:
	case R_X86_64_GOTPCREL:
	case R_X86_64_REX_GOTPCRELX:
	case R_X86_64_GOTPCRELX:
		if (is_undef_sym(s)) {
			/* This is an undefined symbol,
			 * use jmp table as the GOT */
			val += sizeof(unsigned long);
		} else if (GELF_ST_TYPE(s->st_info) == STT_TLS) {
			/* This is GOTTPOFF that already points
			 * to an appropriate GOT entry in the
			 * patient's memory.
			 */
			val = r->r_addend + task->ftrace_trampoline_obj->start - 4;
		}
		/* FALLTHROUGH */
	case R_X86_64_PC32:
	case R_X86_64_PLT32:
		val -= (unsigned long)loc2;
		*(unsigned int *)loc = val;
		break;
	case R_X86_64_PC64:
		val -= (unsigned long)loc2;
		*(unsigned long *)loc = val;
		break;
	case R_X86_64_TPOFF64:
	case R_X86_64_TPOFF32:
		lerror("TPOFF32/TPOFF64 should not be present\n");
		break;
	default:
		lerror("unknown relocation type: %lx\n", ELF64_R_TYPE(r->r_info));
		return -1;
	}
	return 0;

overflow:
    lerror("overflow in relocation type %d val %lx\n",
        (int)ELF64_R_TYPE(r->r_info), val);

	return -ENOEXEC;
}

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
static int is_our_section(GElf_Shdr *s)
{
	// FIXME: is this enough???
	return s->sh_type != SHT_NOBITS;
}

static unsigned long
resolve_undefined(struct process *task, char *sname)
{
    ldebug("Undefined: %s\n", sname);
    //TODO
    return 0;
}

unsigned long
add_jmp_entry(struct process *task, unsigned long jmp, unsigned long addr)
{
    //TODO
    return 0;
}
static inline int
symbol_resolve(struct process *task,
	       GElf_Shdr *shdr,
	       GElf_Sym *s,
	       char *symname)
{
	unsigned long uaddr;

	switch(GELF_ST_TYPE(s->st_info)) {
	case STT_SECTION:
		s->st_value = shdr[s->st_shndx].sh_addr;
		break;

    /* object.o's global function and variable */
	case STT_FUNC:
	case STT_OBJECT:

		/* TODO(pboldin) this breaks rule for overriding
		 * symbols via dynamic libraries. Fix it.
         */
		if (s->st_shndx == SHN_UNDEF &&
		    GELF_ST_BIND(s->st_info) == STB_GLOBAL) {
            lwarning("symbol '%s' is undefined and global, we don't check for overrition\n",
                    symname);
			/* This is a reference to a symbol from
			 * the dynamic library. Resolve it.
             */
			uaddr = resolve_undefined(task, symname);

			if (!uaddr) {
				lerror("Failed to resolve undefined symbol '%s'\n",
				      symname);
				return -1;
			}
			/* OK, we overuse st_size to store original offset */
			s->st_size = uaddr;
			s->st_value = add_jmp_entry(task, JMP_TABLE_JUMP_ARCH, uaddr);

			ldebug("symbol '%s' = 0x%lx\n", symname, uaddr);
			ldebug("jmptable '%s' = 0x%lx\n", symname, s->st_value);
		} else {
			/*
			 * We retain all non-local (except for .LC*)
			 * symbols to help us debugging.
			 */
			if (GELF_ST_BIND(s->st_info) == STB_GLOBAL)
				lwarning("symbol '%s' is defined and global, we don't check for overrition\n",
				       symname);

			s->st_value += shdr[s->st_shndx].sh_addr;
			ldebug("symbol '%s' = 0x%lx\n", symname, s->st_value);
		}

		break;

	case STT_TLS:
		break;

    /* printf, fprintf in here. */
	case STT_NOTYPE:
		s->st_value += shdr[s->st_shndx].sh_addr;
		break;

    /* If obj.o file, skip */
	case STT_FILE:
        break;
	default:
		lerror("Unsupported symbol type: %d, symname = %s\n", GELF_ST_TYPE(s->st_info), symname);
		return -1;
	}

	return 0;
}


static int ftrace_trampoline_object_resolve(struct process *task)
{
    int i, symidx = -1, __unused ret;
	GElf_Ehdr *ehdr;
	GElf_Shdr *shdr;
    GElf_Sym *sym;
	char *strsym;

	ehdr = (void *)task->ftrace_trampoline_obj->obj_local;
	shdr = (void *)ehdr + ehdr->e_shoff;

    for (i = 1; i < ehdr->e_shnum; i++) {
		GElf_Shdr *s = shdr + i;
		if (s->sh_type == SHT_SYMTAB) {
			symidx = i;
            ldebug("SHT_SYMTAB, symidx = %d, section: %s\n", symidx, elf_secname(ehdr, s));
        }

		if (is_our_section(s)) {
			/*
			 * For our own sections, we just point sh_addr to
			 * proper offet in *target process* region of memory
			 */
			s->sh_addr = (unsigned long)task->ftrace_trampoline_obj->start + s->sh_offset;
		} else {
			/*
			 * We copy the `sh_addr`esses from the original binary
			 * into the patch during the preparation. Just use
			 * this.
			 */
			if (s->sh_addr)
				s->sh_addr += 0;
		}
		ldebug("section '%s' = 0x%lx\n", elf_secname(ehdr, s), s->sh_addr);
	}
	ldebug("Resolving symbols for OBJ: '%s'\n", task->ftrace_trampoline_obj->name_);

    sym = (void *)ehdr + shdr[symidx].sh_offset;
	strsym = (void *)ehdr + shdr[shdr[symidx].sh_link].sh_offset;
	for (i = 1; i < shdr[symidx].sh_size / sizeof(GElf_Sym); i++) {
	    GElf_Sym *s = sym + i;
		char *symname = strsym + s->st_name;

        ldebug("Resolving symbols '%s'\n", symname);
		ret = symbol_resolve(task, shdr, s, symname);
		if (ret < 0)
		    return ret;
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

    ftrace_trampoline_object_resolve(task);

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