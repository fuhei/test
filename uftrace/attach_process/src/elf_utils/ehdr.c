/**
 * @file ehdr.c
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


/**
 * @brief check ELF format
 * 
 * @param ehdr 
 * @return int 0 if ok, -1 if not ok
 */
int check_elf_ehdr(GElf_Ehdr *ehdr)
{
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3) {
        return -1;
    }
    return 0;
}

const char *elf_e_type_string(GElf_Ehdr *ehdr)
{
	switch (ehdr->e_type) {
	case ET_NONE: return "No file type";
	case ET_REL: return "Relocatable file";
	case ET_EXEC: return "Executable file";
	case ET_DYN: return "Shared object file";
	case ET_CORE: return "Core file";
	case ET_NUM: return "Number of defined types";
	case ET_LOOS: return "OS-specific range start";
	case ET_HIOS: return "OS-specific range end";
	case ET_LOPROC: return "Processor-specific range start";
	case ET_HIPROC: return "Processor-specific range end";
    default: return "Unknown";
	}
}

const char *elf_e_machine_string(GElf_Ehdr *ehdr)
{
    switch (ehdr->e_machine) {
	case EM_NONE:
		return "No machine type";
	case EM_ARM:
		return "Arm";
	case EM_X86_64:
		return "X86_64";
	case EM_M32 ... EM_RCE:
	case EM_FAKE_ALPHA ... EM_TINYJ:
	case EM_PDSP ... EM_NUM:
    default:
		return "Unknown";
	}
}

/**
 * @brief print GElf_Ehdr
 * 
 * @param ehdr 
 */
void print_elf_ehdr(GElf_Ehdr *ehdr)
{
    printf("\033[1;33m ------ GElf_Ehdr ------ \n\033[m");
#define PRINT_I(e)	do { printf("%20s = %6d (%x)\n", #e, ehdr->e, ehdr->e); fflush(stdout); } while(0)
#define PRINT_L(e)	do { printf("%20s = %6ld (%lx)\n", #e, ehdr->e, ehdr->e); fflush(stdout); } while(0)
#define PRINT_S(e)	do { printf("%20s = %6s\n", #e, ehdr->e); fflush(stdout); } while(0)

    PRINT_S(e_ident);

	printf("%20s = %6d  %s\n", "e_type", ehdr->e_type, elf_e_type_string(ehdr));

	fflush(stdout);

	printf("%20s = %6d  %s\n", "e_machine", ehdr->e_machine, elf_e_machine_string(ehdr));

	fflush(stdout);
	PRINT_I(e_version);
	PRINT_L(e_entry);
	PRINT_L(e_phoff);
	PRINT_L(e_shoff);
	PRINT_I(e_flags);
	PRINT_I(e_ehsize);
	PRINT_I(e_phentsize);
	PRINT_I(e_phnum);
	PRINT_I(e_shentsize);
	PRINT_I(e_shnum);
	PRINT_I(e_shstrndx);
#undef PRINT_I
#undef PRINT_L
}