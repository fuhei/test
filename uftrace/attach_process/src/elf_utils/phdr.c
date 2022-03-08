/**
 * @file phdr.c
 * @author Rong Tao (rongtao@cestc.cn)
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


const char * elf_p_type_string(GElf_Phdr *phdr)
{
    switch (phdr->p_type) {
    case PT_NULL: return "Program header table entry unused";
    case PT_LOAD: return "Loadable program segment";
    case PT_DYNAMIC: return "Dynamic linking information";
    case PT_INTERP: return "Program interpreter";
    case PT_NOTE: return "Auxiliary information";
    case PT_SHLIB: return "Reserved";
    case PT_PHDR: return "Entry for header table itself";
    case PT_TLS: return "Thread-local storage segment";
    case PT_NUM: return "Number of defined types";
    case PT_LOOS: return "Start of OS-specific";
    case PT_GNU_EH_FRAME: return "GCC .eh_frame_hdr segment";
    case PT_GNU_STACK: return "Indicates stack executability";
    case PT_GNU_RELRO: return "Read-only after relocation";
#ifdef PT_GNU_PROPERTY
    case PT_GNU_PROPERTY: return "GNU property";
#endif
    //case PT_LOSUNW: return "PT_LOSUNW";
    case PT_SUNWBSS: return "Sun Specific segment";
    case PT_SUNWSTACK: return "Stack segment";
    //case PT_HISUNW: return "PT_HISUNW";
    case PT_HIOS: return "End of OS-specific";
    case PT_LOPROC: return "Start of processor-specific";
    case PT_HIPROC: return "End of processor-specific";
    default: return "Unknown";
    }
}

static void __print_phdr(GElf_Phdr *shdr, void *arg)
{
    print_elf_phdr(shdr);
}

void elf_for_each_phdr(void *filemap, GElf_Ehdr *ehdr, void (*handler)(GElf_Phdr *shdr, void *arg), void *arg)
{
    int i;
    for (i = 0; i < ehdr->e_phnum; i++) {
        GElf_Phdr *phdr = (GElf_Phdr*)(filemap + ehdr->e_phoff + i * ehdr->e_phentsize);
        handler(phdr, arg);
    }
}

void print_elf_all_phdrs(void *filemap, GElf_Ehdr *ehdr)
{
    printf("\033[1;32m%-8s %-16s %-16s %-8s %-8s %-8s %-8s %s\033[m\n",
            "OFFSET", "VADDR", "PADDR", "FILESZ", "MEMSZ", "FLAGS", "ALIGN", "TYPE");
    elf_for_each_phdr(filemap, ehdr, __print_phdr, NULL);
}

void print_elf_phdr(GElf_Phdr *phdr)
{
    printf("%-8ld %-16lx %-16lx %-8ld %-8ld %-8d %-8ld %s\n",
            phdr->p_offset,
            phdr->p_vaddr,
            phdr->p_paddr,
            phdr->p_filesz,
            phdr->p_memsz,
            phdr->p_flags,
            phdr->p_align,
            elf_p_type_string(phdr));
}