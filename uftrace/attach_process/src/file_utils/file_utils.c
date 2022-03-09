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
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <libgen.h>
#include <getopt.h>

#include "util.h"
#include "log.h"
#include "elf_utils.h"
#include "file_utils.h"
#include "maps.h"
#include "process.h"
#include "syscalls.h"
#include "ftrace.h"
#include "patch.h"
#include "ftrace.h"

int file_size(char *file, ssize_t *size)
{
    int ret, fd;
    struct stat statbuf;

	fd = open(file, O_RDONLY);
    if (fd <= 0) {
        lerror("open %s failed.\n", file, strerror(errno));
        return -1;
    }
    ret = fstat(fd, &statbuf);
    if (ret != 0) {
        lerror("fstat %s failed.\n", file, strerror(errno));
        return -1;
    }
    *size = statbuf.st_size;

    return 0;
}

int file_mktemp(char *tmppath)
{
    int fd;
    fd = mkstemp(tmppath);
	close(fd);
	unlink(tmppath);

    return 0;
}
