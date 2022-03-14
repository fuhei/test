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
#include <malloc.h>
#include <errno.h>

#include "log.h"
#include "file_utils.h"

static int mmap_file_internal(char *filename, void **pmap, ssize_t *plen, int flags, int prot, bool newmalloc)
{
    int fd;

    void *mmap_addr;

    fd = open(filename, flags);
    if (fd <= 0) {
        lerror("open %s failed, %s\n", filename, strerror(errno));
        return -1;
    }
    file_size(filename, plen);
    mmap_addr = mmap(NULL, *plen, prot, MAP_PRIVATE, fd, 0);
    close(fd);

    if (mmap_addr == MAP_FAILED) {
        return -1;
    }

    if (newmalloc) {
        *pmap = malloc(*plen);
        memcpy(*pmap, mmap_addr, *plen);
        munmap_file(mmap_addr, *plen);
    } else {
        *pmap = mmap_addr;
    }

    return 0;
}

int mmap_file_rdonly(char *filename, void **pmap, ssize_t *plen)
{
    return mmap_file_internal(filename, pmap, plen, O_RDONLY, PROT_READ, false);
}

int mmap_file_and_copy(char *filename, void **pmap, ssize_t *plen)
{
    return mmap_file_internal(filename, pmap, plen, O_RDONLY, PROT_READ, true);
}

int munmap_file(char *map, ssize_t len)
{
    return munmap(map, len);
}