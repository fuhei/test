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

#include <gelf.h>
#include "rbtree.h"

struct process;

struct elf_file_sym_struct {
    GElf_Sym *sym;
    char *symname;
    /* Node of struct elf_file_struct.syms_rb */
    struct rb_node sym_node;
};

/* For COMM ELF file */
struct elf_file_struct {
    union {
        GElf_Ehdr *ehdr;
        void *file_map;
    };
    ssize_t file_len;

    GElf_Shdr *symtab_shdr;
    GElf_Sym *symtab;
    /* Root of struct elf_file_sym_struct.sym_node */
    struct rb_root syms_rb;
};

int test_elf(struct process *task);

/* Enum String */
const char *elf_e_machine_string(GElf_Ehdr *ehdr);
const char *elf_e_type_string(GElf_Ehdr *ehdr);
const char *elf_p_type_string(GElf_Phdr *phdr);
const char *elf_sh_name_string(GElf_Shdr *shdr);
const char *elf_sh_type_string(GElf_Shdr *shdr);
const char *elf_sh_flags_string(GElf_Shdr *shdr, void *buff, ssize_t buff_len);

/* Name String */
char *elf_secname(GElf_Ehdr *ehdr, GElf_Shdr *shdr);

/* Count */
int elf_sym_count_undefined(void *filemap, GElf_Ehdr *ehdr);

/* Ftrace */
int check_elf_ftrace_function(void *elf_file_mmap);

/* Ehdr */
int check_elf_ehdr(GElf_Ehdr *ehdr);
void print_elf_ehdr(GElf_Ehdr *ehdr);

/* Phdr */
void elf_for_each_phdr(void *filemap, GElf_Ehdr *ehdr, void (*handler)(GElf_Phdr *shdr, void *arg), void *arg);
void print_elf_phdr(GElf_Phdr *phdr);
void print_elf_all_phdrs(void *filemap, GElf_Ehdr *ehdr);

/* Shdr */
void elf_for_each_shdr(void *filemap, GElf_Ehdr *ehdr, void (*handler)(GElf_Shdr *shdr, void *arg), void *arg);
void print_elf_shdr(GElf_Shdr *shdr);
void print_elf_all_shdrs(void *filemap, GElf_Ehdr *ehdr);

/* Sym */
int elf_for_each_sym(void *filemap, GElf_Ehdr *ehdr,
        void (*handler)(GElf_Shdr *shdr, GElf_Sym *symtab, GElf_Sym *sym, char *symname, void *arg), void *arg);
void print_elf_sym(GElf_Sym *shm);
void print_elf_all_syms(void *filemap, GElf_Ehdr *ehdr);
int is_undef_sym(GElf_Sym *sym);

    /* ELF file Sym */
int elf_file_sym_rbtree_init(struct process *task);
struct elf_file_sym_struct *
elf_file_find_sym(struct process *task, char *symname);

/* Rela */
const char *elf_r_x86_64_name(int r);
void print_elf_rela(GElf_Rela *rela);