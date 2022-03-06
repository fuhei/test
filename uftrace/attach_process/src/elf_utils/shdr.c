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
#include <errno.h>

#include "util.h"
#include "log.h"
#include "maps.h"
#include "elf_utils.h"
#include "compiler.h"


void print_elf_all_shdrs(void *filemap, GElf_Ehdr *ehdr)
{
    int i;
    printf("\033[1;34m%-8s %-16s %-8s %-16s %-8s %-8s %-8s %-8s %-8s %-8s\033[m\n",
            "NAME", "TYPE", "FLAGS", "ADDR", "OFFSET", "SIZE", "LINK", "INFO", "ADDRALIGN", "ENTSZ");
    for (i = 0; i < ehdr->e_shnum; i++) {
        GElf_Shdr *shdr = (GElf_Shdr*)(filemap + ehdr->e_shoff + i * ehdr->e_shentsize);
        print_elf_shdr(shdr);
    }
}

const char *elf_sh_name_string(GElf_Shdr *shdr)
{
    switch (shdr->sh_name) {
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

void print_elf_shdr(GElf_Shdr *shdr)
{
    printf("%-8d %-16d %-8ld %-16lx %-8ld %-8ld %-8d %-8d %-8ld %-8ld\n",
            shdr->sh_name,
            shdr->sh_type,
            shdr->sh_flags,
            shdr->sh_addr,
            shdr->sh_offset,
            shdr->sh_size,
            shdr->sh_link,
            shdr->sh_info,
            shdr->sh_addralign,
            shdr->sh_entsize);
}
