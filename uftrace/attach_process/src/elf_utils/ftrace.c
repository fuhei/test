/**
 * @file ftrace.c
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
#include "elf_utils.h"
#include "compiler.h"

const static char __unused *___ftrace_funcs[] = {
    "__cyg_profile_func_enter",
    "__fentry__",
    "mcount",
    "_mcount",
    "__gnu_mcount_nc",
};


int elf_check_ftrace_function(struct elf_file_struct *elf_file)
{
    int i;
    GElf_Ehdr *ehdr;
    GElf_Shdr *shdr, *dynsym_shdr = NULL;
    GElf_Sym *isym;

    char *file_map = (char*)elf_file->file_map;

    ehdr = (GElf_Ehdr *)file_map;

    shdr = (void*)file_map + ehdr->e_shoff;
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
