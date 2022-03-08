/**
 * @file attach.c
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief
 * @version 0.1
 * @date 2022-03-03
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
#include "maps.h"
#include "process.h"
#include "syscalls.h"
#include "ftrace.h"
#include "patch.h"

static struct args_opts default_args_opts = {
	.verbose_flag = -1,
	.target_pid = 0,
	.target_func = NULL,
	.check_trace_func = true,
};

static struct option args_options[] =
{
	{"verbose",		no_argument,		&default_args_opts.verbose_flag,	1},
	{"no-check-trace",	no_argument,	0,				2},
	{"target-function",	required_argument,	0,			'f'},
	{"pid",			required_argument,	0,				'p'},
	{"help",		no_argument,		0,				'h'},
	{0,0,0,0},
};

static void usage_help(char *cmd)
{
	printf("Usage: %s\n", cmd);
	printf(" \n");
	printf(" Options:\n");
	printf("  -p, --pid	specify a target pid.\n");
	printf("  -f, --target-function	specify a target function to trace.\n");
	printf(" \n");
	printf(" Other options:\n");
	printf("  --verbose	show detail.\n");
	printf("  --no-check-trace		Do not check trace function, like mcount(), default is CHECK.\n");
	printf(" \n");
	exit(1);
}

void sig_handler(int signum)
{
	exit(1);
}

#if 0
void parse_args(int argc, char *argv[])
{
	pid_t pid = 0;
	while (1) {
        /* getopt_long stores the option index here. */
        int c, option_index = 0;
        c = getopt_long (argc, argv, "f:hp:", args_options, &option_index);
        if (c < 0) {
            break;
        }
		switch (c) {
		case 1:
			default_args_opts.verbose_flag = 1;
			break;
		case 2:
			default_args_opts.check_trace_func = false;
			break;
		case 'h':
			usage_help(argv[0]);
			break;
		case 'f':
			default_args_opts.target_func = strdup(optarg);
			break;
		case 'p':
			pid = atoi(optarg);
			break;
		default:
			usage_help(argv[0]);
        }
	}
	if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }
	if (default_args_opts.verbose_flag == 1) {
		puts("verbose flag is set");
	} else if(default_args_opts.verbose_flag == 0) {
		puts("verbose flag is not set");
	}

	if (pid == 0) {
		lerror("no set -p, --pid option\n");
		usage_help(argv[0]);
	}
	default_args_opts.target_pid = pid;

	if (!default_args_opts.target_func) {
		lerror("Specify -f function option\n");
	}
}
#endif

int main(int argc, char *argv[])
{
	int ret = -1;
	pid_t pid = 0;
	struct process test_p;

	while (1) {
        /* getopt_long stores the option index here. */
        int c, option_index = 0;
        c = getopt_long (argc, argv, "f:hp:", args_options, &option_index);
        if (c < 0) {
            break;
        }
		switch (c) {
		case 1:
			default_args_opts.verbose_flag = 1;
			break;
		case 2:
			default_args_opts.check_trace_func = false;
			break;
		case 'h':
			usage_help(argv[0]);
			break;
		case 'f':
			default_args_opts.target_func = strdup(optarg);
			break;
		case 'p':
			pid = atoi(optarg);
			break;
		default:
			usage_help(argv[0]);
        }
	}
	if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }
	if (default_args_opts.verbose_flag == 1) {
		puts("verbose flag is set");
	} else if(default_args_opts.verbose_flag == 0) {
		puts("verbose flag is not set");
	}

	if (pid == 0) {
		lerror("no set -p, --pid option\n");
		usage_help(argv[0]);
	}
	default_args_opts.target_pid = pid;

	if (!default_args_opts.target_func) {
		lerror("Specify -f function option\n");
	}

	signal(SIGINT, sig_handler);

	/* Open target process */
	ret = open_process(&test_p, default_args_opts.target_pid, &default_args_opts);
	if (ret != 0)
		goto open_fail_return;

	/* Attach target Process */
	ret = process_attach(&test_p);
	if (ret != 0) {
		lerror("attach %d failed, ret = %d\n", test_p.pid, ret);
		goto attach_fail_return;
	}

#if 0
	test_syscall_mmap(&test_p);
	test_maps_vma(&test_p);
	test_elf(&test_p);

	/* see tests/single-ftrace-upatch */
	test_patch(&test_p);
	/* see tests/single-ftrace-upatch */
	test_ftrace(&test_p);
#endif


	usleep(100);
	process_detach(&test_p);

	while (1) {
		sleep(1);
	}

attach_fail_return:
	close_process(&test_p);
open_fail_return:
	return ret;
}
