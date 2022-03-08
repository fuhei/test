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

int mmap_file(char *filename, void **pmap, ssize_t *plen);
int munmap_file(char *map, ssize_t len);