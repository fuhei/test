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
#if defined(__x86_64__)
#include "arch/x86_64/instruments.h"
#elif defined(__aarch64__)
#include "arch/aarch64/instruments.h"
#endif

static struct args_opts default_args_opts = {
	.verbose_flag = -1,
	.target_pid = 0,
	.target_func = NULL,
	.check_trace_func = true,
	.ftrace_object = NULL,
};

static struct option args_options[] =
{
	{"verbose",		no_argument,		&default_args_opts.verbose_flag,	1},
	{"no-check-trace",	no_argument,	0,				2},
	{"ftrace-object",	required_argument, 0,			3},
	{"target-function",	required_argument,	0,			'f'},
	{"log-level",	required_argument,	0,				'l'},
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
	printf("  -l, --log-level	Log level, see /usr/include/syslog.h\n");
	printf("			emerg(%d), alert(%d), crit(%d), err(%d), warn(%d), notice(%d), info(%d), debug(%d)\n",
						LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR, LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG);
	printf(" \n");
	printf(" Other options:\n");
	printf("  --verbose	show detail.\n");
	printf("  --ftrace-object		Insert obj.o file for ftrace trampoline.\n");
	printf("  --no-check-trace		Do not check trace function, like mcount(), default is CHECK.\n");
	printf(" \n");
	exit(1);
}

int parse_args(int argc, char *argv[])
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
		case 3:
			default_args_opts.ftrace_object = optarg;
			break;
		case 'h':
			usage_help(argv[0]);
			break;
		case 'f':
			default_args_opts.target_func = strdup(optarg);
			break;
		case 'l':
			set_log_level(atoi(optarg));
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

	return 0;
}


void sig_handler(int signum)
{
	exit(1);
}

int main(int argc, char *argv[])
{
	int ret = -1;
	struct process test_p;

	signal(SIGINT, sig_handler);

	parse_args(argc, argv);

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

#if 1
	/* Insert trampoline object */
	if (default_args_opts.ftrace_object) {

		test_maps_vma(&test_p);

		ftrace_insert_trampoline_object(&test_p, default_args_opts.ftrace_object);
		ftrace_trampoline_object_relocate(&test_p);

		memcpy_to_remote(&test_p,
					test_p.ftrace_trampoline_obj->start,
					test_p.ftrace_trampoline_obj->local_elf_file->obj_local,
					test_p.ftrace_trampoline_obj->local_elf_file->obj_size);

#if 1
		char buffer[2048] = {0};
		memcpy_from_remote(&test_p,
					buffer,
					test_p.ftrace_trampoline_obj->start,
					test_p.ftrace_trampoline_obj->local_elf_file->obj_size);
		GElf_Ehdr *__ehdr = (void*)buffer;
		if (!check_elf_ehdr_magic(__ehdr)) {
			lwarning("Buffer is ELF, %s\n", __ehdr->e_ident);
		}
#endif
#if 0
		unsigned char orginal_code[5];
		unsigned char code[] = {INST_CALLQ, 0x00, 0x00, 0x00, 0x00};
		struct elf_file_sym_struct *func_sym;
		struct process *task = &test_p;

		func_sym = elf_file_find_sym(task, "func2");

		if (!func_sym) {
			lerror("No find func_sym\n");
			exit(1);
		}
		lwarning("func_sym: %16lx\n", func_sym->sym->st_value);

		*(unsigned int *)(code + 1) = (unsigned int)(0x405040 - (func_sym->sym->st_value) - 9);

		/* Get original code */
		memcpy_from_remote(task, &orginal_code, func_sym->sym->st_value + 4, sizeof(orginal_code));

		/* Set jmp function */
		memcpy_to_remote(task, func_sym->sym->st_value + 4, &code, sizeof(code));

		process_detach(task);

		/* Make target process running */
		sleep(10);

		process_attach(task);

		/* Recover */
		memcpy_to_remote(task, func_sym->sym->st_value + 4, &orginal_code, sizeof(code));
#endif
	}
#endif

#if 0
	test_syscall_mmap(&test_p);
	test_maps_vma(&test_p);
	test_elf(&test_p);
#endif
#if 0
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
