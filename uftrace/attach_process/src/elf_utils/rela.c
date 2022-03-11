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


static const char *R_X86_64_STRING[R_X86_64_NUM] = {
#define _I(v) [v] = #v
    _I(R_X86_64_NONE),
    _I(R_X86_64_64),
    _I(R_X86_64_PC32),
    _I(R_X86_64_GOT32),
    _I(R_X86_64_PLT32),
    _I(R_X86_64_COPY),
    _I(R_X86_64_GLOB_DAT),
    _I(R_X86_64_JUMP_SLOT),
    _I(R_X86_64_RELATIVE),
    _I(R_X86_64_GOTPCREL),
    _I(R_X86_64_32),
    _I(R_X86_64_32S),
    _I(R_X86_64_16),
    _I(R_X86_64_PC16),
    _I(R_X86_64_8),
    _I(R_X86_64_PC8),
    _I(R_X86_64_DTPMOD64),
    _I(R_X86_64_DTPOFF64),
    _I(R_X86_64_TPOFF64),
    _I(R_X86_64_TLSGD),
    _I(R_X86_64_TLSLD),
    _I(R_X86_64_DTPOFF32),
    _I(R_X86_64_GOTTPOFF),
    _I(R_X86_64_TPOFF32),
    _I(R_X86_64_PC64),
    _I(R_X86_64_GOTOFF64),
    _I(R_X86_64_GOTPC32),
    _I(R_X86_64_GOT64),
    _I(R_X86_64_GOTPCREL64),
    _I(R_X86_64_GOTPC64),
    _I(R_X86_64_GOTPLT64),
    _I(R_X86_64_PLTOFF64),
    _I(R_X86_64_SIZE32),
    _I(R_X86_64_SIZE64),
    _I(R_X86_64_GOTPC32_TLSDESC),
    _I(R_X86_64_TLSDESC_CALL),
    _I(R_X86_64_TLSDESC),
    _I(R_X86_64_IRELATIVE),
    _I(R_X86_64_RELATIVE64),
    _I(R_X86_64_GOTPCRELX),
    _I(R_X86_64_REX_GOTPCRELX),
#undef _I
};

const char *elf_r_x86_64_name(int r)
{
    return R_X86_64_STRING[r];
}

void print_elf_rela(GElf_Rela *rela)
{
    printf("%016lx %16ld %16ld %16ld\n", rela->r_offset, rela->r_info, GELF_R_TYPE(rela->r_info), rela->r_addend);
    //TODO
}