/**
 * @file elf_utils.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-03
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
#include "elf_utils.h"
#include "process.h"
#include "compiler.h"


const static char __unused *___ftrace_funcs[] = { 
    "__cyg_profile_func_enter",
    "__fentry__",
    "mcount",
    "_mcount",
    "__gnu_mcount_nc",
};


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

int check_elf_ftrace_function(void *elf_file_mmap)
{
    int i;
    GElf_Ehdr *ehdr;
    GElf_Shdr *shdr, *dynsym_shdr = NULL;
    GElf_Sym *isym;

    char *file_map = (char*)elf_file_mmap;

    ehdr = (GElf_Ehdr *)file_map;

    shdr = elf_file_mmap + ehdr->e_shoff;
    for (i = 0; i < ehdr->e_shnum; i++) {
        GElf_Shdr *ishdr = (GElf_Shdr *)(file_map + ehdr->e_shoff + i * ehdr->e_shentsize);
        if (ishdr->sh_type == SHT_DYNSYM) {
            dynsym_shdr = ishdr;
            break;
        }
    }
    if (!dynsym_shdr) {
        lerror("cannot find dynamic symbols.. skipping\n");
        return -1;
    }

    char *dynsym_strtab = file_map + shdr[dynsym_shdr->sh_link].sh_offset;
    for (i = 0; i < dynsym_shdr->sh_size / dynsym_shdr->sh_entsize && dynsym_shdr->sh_type == SHT_DYNSYM; i++) {
        int j;
        char *data = file_map + dynsym_shdr->sh_offset;
        isym = (GElf_Sym *)&data[i * dynsym_shdr->sh_entsize];

        char *name = (char*)(dynsym_strtab + isym->st_name);

        if (GELF_ST_TYPE(isym->st_info) != STT_FUNC &&
            GELF_ST_TYPE(isym->st_info) != STT_GNU_IFUNC) {
            continue;
        }
        for (j = 0; j < ARRAY_SIZE(___ftrace_funcs); j++) {
            if (!strcmp(name, ___ftrace_funcs[j]))
                return 0;
        }
    }

    return -1;
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
    case PT_GNU_PROPERTY: return "GNU property";
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


void print_elf_all_phdrs(void *filemap, GElf_Ehdr *ehdr)
{
    int i;
    printf("\033[1;32m%-8s %-16s %-16s %-8s %-8s %-8s %-8s %s\033[m\n",
            "OFFSET", "VADDR", "PADDR", "FILESZ", "MEMSZ", "FLAGS", "ALIGN", "TYPE");
    for (i = 0; i < ehdr->e_phnum; i++) {
        GElf_Phdr *phdr = (GElf_Phdr*)(filemap + ehdr->e_phoff + i * ehdr->e_phentsize);
        print_elf_phdr(phdr);
    }
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

void print_elf_all_shdrs(void *filemap, GElf_Ehdr *ehdr)
{
    int i;
    printf("\033[1;34m%-16s %-16s %-8s %-16s %-8s %-8s %-8s %-8s %-8s %-8s\033[m\n",
            "NAME", "TYPE", "FLAGS", "ADDR", "OFFSET", "SIZE", "LINK", "INFO", "ADDRALIGN", "ENTSZ");
    for (i = 0; i < ehdr->e_shnum; i++) {
        GElf_Shdr *shdr = (GElf_Shdr*)(filemap + ehdr->e_shoff + i * ehdr->e_shentsize);
        print_elf_shdr(shdr);
    }
}

void print_elf_shdr(GElf_Shdr *shdr)
{
    printf("%-16d %-16d %-8ld %-16lx %-8ld %-8ld %-8d %-8d %-8ld %-8ld\n",
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
