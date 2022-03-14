/**
 * @file file_utils.h
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <stdbool.h>

int mmap_file_rdonly(char *filename, void **pmap, ssize_t *plen);
int mmap_file_and_copy(char *filename, void **pmap, ssize_t *plen);
int munmap_file(char *map, ssize_t len);

int file_size(char *file, ssize_t *size);
int file_mktemp(char *tmppath);