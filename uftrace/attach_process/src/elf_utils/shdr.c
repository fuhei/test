/**
 * @file shdr.c
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
#include <malloc.h>
#include <errno.h>

#include "util.h"
#include "log.h"
#include "maps.h"
#include "elf_utils.h"
#include "compiler.h"


char *elf_secname(GElf_Ehdr *ehdr, GElf_Shdr *s)
{
	GElf_Shdr *shdr = (void *)ehdr + ehdr->e_shoff;
	char *str = (void *)ehdr + shdr[ehdr->e_shstrndx].sh_offset;

	return str + s->sh_name;
}

static void __print_shdr(GElf_Shdr *shdr, void *arg)
{
    print_elf_shdr(shdr);
}

void elf_for_each_shdr(void *filemap, GElf_Ehdr *ehdr, void (*handler)(GElf_Shdr *shdr, void *arg), void *arg)
{
    int i;
    for (i = 0; i < ehdr->e_shnum; i++) {
        GElf_Shdr *shdr = (GElf_Shdr*)(filemap + ehdr->e_shoff + i * ehdr->e_shentsize);
        handler(shdr, arg);
    }
}

void print_elf_all_shdrs(void *filemap, GElf_Ehdr *ehdr)
{
    printf("\033[1;34m ----- ALL Elf_Shdr -----\033[m\n");
    elf_for_each_shdr(filemap, ehdr, __print_shdr, NULL);
}

const char *elf_sh_name_string(GElf_Shdr *shdr)
{
    switch (shdr->sh_name) {
    case SHN_UNDEF: return "Undefined section";
    case SHN_LORESERVE: return "Start of reserved indices";
    //case SHN_LOPROC: return "Start of processor-specific";
    //case SHN_BEFORE: return "Order section before all others(Solaris).";
    case SHN_AFTER: return "Order section after all others(Solaris).";
    case SHN_HIPROC: return "End of processor-specific";
    case SHN_LOOS: return "Start of OS-specific";
    case SHN_HIOS: return "End of OS-specific";
    case SHN_ABS: return "Associated symbol is absolute";
    case SHN_COMMON: return "Associated symbol is common";
    case SHN_XINDEX: return "Index is in extra table.";
    //case SHN_HIRESERVE: return "End of reserved indices";
    default: return "Unknown";
    }
}

const char *elf_sh_type_string(GElf_Shdr *shdr)
{
    switch (shdr->sh_type) {
    case SHT_NULL: return "Section header table entry unused";
    case SHT_PROGBITS: return "Program data";
    case SHT_SYMTAB: return "Symbol table";
    case SHT_STRTAB: return "String table";
    case SHT_RELA: return "Relocation entries with addends";
    case SHT_HASH: return "Symbol hash table";
    case SHT_DYNAMIC: return "Dynamic linking information";
    case SHT_NOTE: return "Notes";
    case SHT_NOBITS: return "Program space with no data";
    case SHT_REL: return "Relocation entries, no addends";
    case SHT_SHLIB: return "Reserved";
    case SHT_DYNSYM: return "Dynamic linker symbol table";
    case SHT_INIT_ARRAY: return "Array of constructors ";
    case SHT_FINI_ARRAY: return "Array of destructors";
    case SHT_PREINIT_ARRAY: return "Array of pre-constructors";
    case SHT_GROUP: return "Section group";
    case SHT_SYMTAB_SHNDX: return "Extended section indices";
    case SHT_NUM: return "Number of defined types.";
    case SHT_LOOS: return "Start OS-specific.";
    case SHT_GNU_ATTRIBUTES: return "Object attributes.";
    case SHT_GNU_HASH: return "GNU-style hash table.";
    case SHT_GNU_LIBLIST: return "Prelink library list";
    case SHT_CHECKSUM: return "Checksum for DSO content.";
    //case SHT_LOSUNW: return "Sun-specific low bound.  ";
    case SHT_SUNW_move: return "SHT_SUNW_move";
    case SHT_SUNW_COMDAT: return "SHT_SUNW_COMDAT";
    case SHT_SUNW_syminfo: return "SHT_SUNW_syminfo";
    case SHT_GNU_verdef: return "Version definition section.";
    case SHT_GNU_verneed: return "Version needs section.";
    case SHT_GNU_versym: return "Version symbol table.";
    //case SHT_HISUNW: return "Sun-specific high bound.";
    //case SHT_HIOS: return "End OS-specific type";
    case SHT_LOPROC: return "Start of processor-specific";
    case SHT_HIPROC: return "End of processor-specific";
    case SHT_LOUSER: return "Start of application-specific";
    case SHT_HIUSER: return "End of application-specific";
    default: return "Unknown";
    }
}

const char *elf_sh_flags_string(GElf_Shdr *shdr, void *buff, ssize_t buff_len)
{
    char *flags = (char*)buff;
    if (!flags || buff_len < 33) {
        lerror("buffer invalied.\n");
    }
    memset(flags, '.', sizeof(char)*32);
    flags[32] = '\0';

    /* Writable */
    if (shdr->sh_flags & SHF_WRITE)
        flags[32 - 1 - 0] = 'W';
    /* Occupies memory during execution */
    if (shdr->sh_flags & SHF_ALLOC)
        flags[32 - 1 - 1] = 'A';
    /* Executable */
    if (shdr->sh_flags & SHF_EXECINSTR)
        flags[32 - 1 - 2] = 'X';
    /* Might be merged */
    if (shdr->sh_flags & SHF_MERGE)
        flags[32 - 1 - 4] = 'M';
    /* Contains nul-terminated strings */
    if (shdr->sh_flags & SHF_STRINGS)
        flags[32 - 1 - 5] = 'S';
    /* `sh_info' contains SHT index */
    if (shdr->sh_flags & SHF_INFO_LINK)
        flags[32 - 1 - 6] = 'I';
    /* Preserve order after combining */
    if (shdr->sh_flags & SHF_LINK_ORDER)
        flags[32 - 1 - 7] = 'L';
    /* Non-standard OS specific handling required */
    if (shdr->sh_flags & SHF_OS_NONCONFORMING)
        flags[32 - 1 - 8] = 'O';
    /* Section is member of a group.  */
    if (shdr->sh_flags & SHF_GROUP)
        flags[32 - 1 - 9] = 'G';
    /* Section hold thread-local data.  */
    if (shdr->sh_flags & SHF_TLS)
        flags[32 - 1 - 10] = 'T';
    /* Section with compressed data. */
    if (shdr->sh_flags & SHF_COMPRESSED)
        flags[32 - 1 - 11] = 'C';
    /* OS-specific.  */
    if (shdr->sh_flags & SHF_MASKOS) {
        //TODO
    }
    /* Processor-specific */
    if (shdr->sh_flags & SHF_MASKPROC) {
        //TODO
    }
    /* Not to be GCed by linker.  */
    if (shdr->sh_flags & SHF_GNU_RETAIN)
        flags[32 - 1 - 21] = 'g';
    /* Special ordering requirement(Solaris).  */
    if (shdr->sh_flags & SHF_ORDERED)
        flags[32 - 1 - 30] = 'o';
    /* Section is excluded unless referenced or allocated (Solaris).*/
    if (shdr->sh_flags & SHF_EXCLUDE)
        flags[32 - 1 - 31] = 'E';

    return flags;
}

void print_elf_shdr(GElf_Shdr *shdr)
{
    char buffer[64];
    printf("\033[1;33m ------------------------\033[m\n");
    printf("NAME: %s\n", elf_sh_name_string(shdr));
    printf("TYPE: %s\n", elf_sh_type_string(shdr));
    printf("FLAG: %s\n", elf_sh_flags_string(shdr, buffer, sizeof(buffer)));
    printf("%-16s %-8s %-8s %-8s %-8s %-8s %-8s\n",
            "ADDR", "OFFSET", "SIZE", "LINK", "INFO", "ALIGN", "ENTSIZE");
    printf("%-16lx %-8ld %-8ld %-8d %-8d %-8ld %-8ld\n",
            shdr->sh_addr,
            shdr->sh_offset,
            shdr->sh_size,
            shdr->sh_link,
            shdr->sh_info,
            shdr->sh_addralign,
            shdr->sh_entsize);
}
