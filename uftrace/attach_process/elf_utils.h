/**
 * @file elf_utils.h
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <gelf.h>


int check_elf_ehdr(GElf_Ehdr *ehdr);


const char *elf_e_machine_string(GElf_Ehdr *ehdr);
void print_elf_ehdr(GElf_Ehdr *ehdr);

const char * elf_p_type_string(GElf_Phdr *phdr);
void print_elf_phdr(GElf_Phdr *phdr);
void print_elf_all_phdrs(void *filemap, GElf_Ehdr *ehdr);

void print_elf_shdr(GElf_Shdr *shdr);
void print_elf_all_shdrs(void *filemap, GElf_Ehdr *ehdr);