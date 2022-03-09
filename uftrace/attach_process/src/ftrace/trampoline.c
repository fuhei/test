/**
 * @file trampoline.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-07
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
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


int ftrace_insert_trampoline_object(struct process *task, char *objpath)
{
    char objfile[512] = {0};
    ssize_t size;

    file_size(objpath, &size);

    snprintf(objfile, sizeof(objfile), "/tmp/trampoline-%s.XXXXXX", basename(objpath));
    file_mktemp(objfile);

    return remote_insert_object(task, objfile, size, VMA_FTRACE_TRAMPOLINE);
}