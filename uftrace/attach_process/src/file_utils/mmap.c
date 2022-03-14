/**
 * @file mmap.c
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

#include "log.h"
#include "file_utils.h"

static int mmap_file_internal(char *filename, void **pmap, ssize_t *plen, int flags, int prot)
{
    int fd;

    fd = open(filename, flags);
    if (fd <= 0) {
        lerror("open %s failed, %s\n", filename, strerror(errno));
        return -1;
    }
    file_size(filename, plen);
    *pmap = mmap(NULL, *plen, prot, MAP_PRIVATE, fd, 0);
    close(fd);

    if (*pmap == MAP_FAILED) {
        return -1;
    }
    return 0;
}

int mmap_file_rdonly(char *filename, void **pmap, ssize_t *plen)
{
    return mmap_file_internal(filename, pmap, plen, O_RDONLY, PROT_READ);
}

int munmap_file(char *map, ssize_t len)
{
    return munmap(map, len);
}