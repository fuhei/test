/**
 * @file log.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdarg.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

static const char *level_prefix[] = {
    "[\033[1;5;31mEMERG\033[m]",
    "[\033[1;5;31mALERT\033[m]",
      "[\033[1;31m CRIT\033[m]",
      "[\033[1;31mERROR\033[m]",
      "[\033[1;32m WARN\033[m]",
      "[\033[1;33mNOTIC\033[m]",
      "[\033[1;34m INFO\033[m]",
      "[\033[1;35mDEBUG\033[m]",
};

int
_____log(int level, const char *file, const char *func, unsigned long int line, char *fmt, ...)
{
    int n = 0;
    FILE *fp = stdout;
    va_list va;

    if (level <= LOG_ERR)
        fp = stderr;

    va_start(va, fmt);

    fprintf(fp, "%s[%s:%ld] ", level_prefix[level], func, line);
    n += vfprintf(fp, fmt, va);

    va_end(va);

    if (level <= LOG_ERR) {
        exit(1);
    }

    return n;
}

int test_log(void)
{
	ldebug("debug %d\n", 1);
	linfo("info %d\n", 1);
	lnotice("notice %d\n", 1);
	lwarning("warning %d\n", 1);
	lerror("error %d\n", 1);
	lcrit("crit %d\n", 1);
	lalert("alert %d\n", 1);
	lemerg("emerg %d\n", 1);
    return 0;
}