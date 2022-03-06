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


static struct args_opts default_args_opts = {
	.verbose_flag = -1,
	.check_trace_func = true,
};

static struct option args_options[] =
{
	{"verbose",		no_argument,		&default_args_opts.verbose_flag,	1},
	{"no-check-trace",	no_argument,	0,				2},
	{"pid",			required_argument,	0,				'p'},
	{"help",		no_argument,		0,				'h'},
	{0,0,0,0},
};

static void usage_help(char *cmd)
{
	lerror("Usage: %s\n", cmd);
	lerror(" \n");
	lerror(" Options:\n");
	lerror("  -p, --pid	specify a target pid.\n");
	lerror(" \n");
	lerror(" Other options:\n");
	lerror("  --verbose	show detail.\n");
	lerror("  --no-check-trace		Do not check trace function, like mcount(), default is CHECK.\n");
	lerror(" \n");
	exit(1);
}

void sig_handler(int signum)
{
	exit(1);
}

int main(int argc, char *argv[])
{
	int ret = -1;
	pid_t pid = 0;
	struct vma_struct __unused *ivma = NULL;
	struct process test_p;

	while (1) {
        /* getopt_long stores the option index here. */
        int c, option_index = 0;
        c = getopt_long (argc, argv, "hp:", args_options, &option_index);
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
	signal(SIGINT, sig_handler);

	ret = open_process(&test_p, pid, &default_args_opts);
	if (ret != 0)
		goto open_fail_return;

	ret = process_attach(&test_p);
	if (ret != 0) {
		lerror("attach %d failed, ret = %d\n", test_p.pid, ret);
		goto attach_fail_return;
	}

#if 0
	list_for_each_entry(ivma, &test_p.vmas, node) {
		print_vma(ivma);
	}
#endif


#if 0
	list_for_each_entry(ivma, &test_p.vmas, node) {
		GElf_Ehdr *ehdr = (GElf_Ehdr*)ivma->content;
		if (check_elf_ehdr(ehdr) == -1) {
			continue;
		}
		lerror("%s is ELF.\n", ivma->name_);

		print_elf_ehdr(ehdr);

		print_elf_all_phdrs(ivma->content, ehdr);
		//print_elf_all_shdrs(buffer, ehdr);
	}
#endif

#if 0
	printf("----- rbtree -------\n");
	struct rb_node *rnode;
	for (rnode = rb_first(&test_p.vmas_rb); rnode; rnode = rb_next(rnode)) {
		ivma = rb_entry(rnode, struct vma_struct, rb_node);
		print_vma(ivma);
	}
#endif

#if 1
	unsigned long map_v;
	ssize_t map_len = 8192;
	int __unused map_fd;

	map_v = remote_mmap(&test_p,
				0UL, map_len,
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ldebug("New mmap. %lx\n", map_v);
	remote_munmap(&test_p, map_v, map_len);
	sleep(3);

	map_fd = remote_open(&test_p, "todo.txt", O_RDWR|O_CREAT|O_TRUNC, 0644);
	ldebug("New open. %d\n", map_fd);
	remote_close(&test_p, map_fd);
	map_v = remote_mmap(&test_p,
				0UL, 8192*3,
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ldebug("New mmap. %lx\n", map_v);
	remote_munmap(&test_p, map_v, map_len);
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
