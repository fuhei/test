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


static int __apply_relocate_add(Elf64_Shdr *sechdrs,
		   const char *strtab,
		   unsigned int symindex,
		   unsigned int relsec,
		   struct process *task,
		   void *(*write_func)(void *dest, const void *src, size_t len),
		   struct elf_file_struct *elf)
{
	unsigned int i;
	Elf64_Rela *rel = (void *)sechdrs[relsec].sh_addr;
	Elf64_Sym *sym;
	void *loc;
	uint64_t val;

	ldebug("Applying relocate section %u to %u\n",
				relsec, sechdrs[relsec].sh_info);
	for (i = 0; i < sechdrs[relsec].sh_size / sizeof(*rel); i++) {
		/* This is where to make the change */
		loc = (void *)sechdrs[sechdrs[relsec].sh_info].sh_addr
			+ rel[i].r_offset;

		/* This is the symbol it is referring to.  Note that all
		 * undefined symbols have been resolved.
		 */
		sym = (Elf64_Sym *)sechdrs[symindex].sh_addr
			+ ELF64_R_SYM(rel[i].r_info);

		val = sym->st_value + rel[i].r_addend + elf->belongs_to_vma->start - (unsigned long)elf->ehdr;

		ldebug("type %d st_value %Lx r_addend %Lx loc %Lx "\
				"local start %Lx target start %Lx value %Lx\n",
		       (int)ELF64_R_TYPE(rel[i].r_info),
		       sym->st_value, rel[i].r_addend, (uint64_t)loc,
			   (unsigned long)elf->ehdr,
			   elf->belongs_to_vma->start,
			   val);

		switch (ELF64_R_TYPE(rel[i].r_info)) {
		case R_X86_64_NONE:
			break;
        /**
         *  word64  S + A
         */
		case R_X86_64_64:
			if (*(uint64_t *)loc != 0)
				goto invalid_relocation;
			write_func(loc, &val, 8);
			break;
        /**
         *  word32  S + A - P
         */
		case R_X86_64_32:
			if (*(uint32_t *)loc != 0)
				goto invalid_relocation;
			write_func(loc, &val, 4);
			if (val != *(uint32_t *)loc)
				goto overflow;
			break;
        /**
         *  word32  S + A
         */
		case R_X86_64_32S:
			if (*(int32_t *)loc != 0)
				goto invalid_relocation;
			write_func(loc, &val, 4);
			if ((int64_t)val != *(int32_t *)loc)
				goto overflow;
			break;
        /**
         *  word32  S + A - P
         */
		case R_X86_64_PC32:
        /**
         *  word32  L + A - P
         */
		case R_X86_64_PLT32:
			if (*(uint32_t *)loc != 0)
				goto invalid_relocation;
			val -= (uint64_t)loc;
			write_func(loc, &val, 4);
#if 0
			if ((int64_t)val != *(int32_t *)loc)
				goto overflow;
#endif
			break;
        /**
         *  word64  S + A - P
         */
		case R_X86_64_PC64:
			if (*(uint64_t *)loc != 0)
				goto invalid_relocation;
			val -= (uint64_t)loc;
			write_func(loc, &val, 8);
			break;
		default:
			lerror("Unknown rela relocation: %llu\n",
			       ELF64_R_TYPE(rel[i].r_info));
			return -1;
		}
	}
	return 0;
invalid_relocation:
	lerror("x86/modules: Skipping invalid relocation target, existing value is nonzero for type %d, loc %p, val %Lx\n",
	       (int)ELF64_R_TYPE(rel[i].r_info), loc, val);
	return -1;
overflow:
	lerror("overflow in relocation type %d val %Lx\n",
	       (int)ELF64_R_TYPE(rel[i].r_info), val);
	lerror("likely not compiled with -mcmodel=kernel\n");
	return -1;
}


int apply_relocate_add(Elf64_Shdr *sechdrs,
		   const char *strtab,
		   unsigned int symindex,
		   unsigned int relsec,
		   struct process *task,
		   struct elf_file_struct *elf)
{
	int ret;
	void *(*write_fn)(void *, const void *, size_t) = memcpy;

	ret = __apply_relocate_add(sechdrs, strtab, symindex, relsec, task,
				   write_fn, elf);
	return ret;
}


int apply_relocate(Elf64_Shdr *sechdrs,
		   const char *strtab,
		   unsigned int symindex,
		   unsigned int relsec,
		   struct process *me)
{
	/* TODO: 32bit */
	return 0;
}
