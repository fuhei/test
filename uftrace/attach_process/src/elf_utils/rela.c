/**
 * @file rela.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-09
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

void print_elf_rela(GElf_Rela *rela)
{
    printf("%016lx %16ld %16ld %16ld\n", rela->r_offset, rela->r_info, GELF_R_TYPE(rela->r_info), rela->r_addend);
    //TODO
}