/**
 * @file file_utils.c
 * @author  Rong Tao (rongtao@cestc.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-04
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

int mmap_file(char *filename, void **pmap, ssize_t *plen)
{
    struct stat stat;
    int fd;
    
    fd = open(filename, O_RDONLY);
    if (fd <= 0) {
        lerror("open %s failed, %s\n", filename, strerror(errno));
        return -1;
    }
    fstat(fd, &stat);
    *plen = stat.st_size;
    *pmap = mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (*pmap == MAP_FAILED) {
        return -1;
    }
    return 0;
}

int munmap_file(char *map, ssize_t len)
{
    return munmap(map, len);
}