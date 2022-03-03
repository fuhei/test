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

#include "elf_utils.h"
#include "maps.h"
#include "process.h"

const char *filename = "a.out";
pid_t pid;


void sig_handler(int signum)
{
	kill(pid, SIGINT);
	exit(1);
}


int main(int argc, char *argv[])
{
	struct vma_struct *ivma = NULL, *tmpvma;
	struct process test_p;
	
	signal(SIGINT, sig_handler);

	pid = fork();
	if (pid == 0) { //Child
		char *__argv[] = {
			"-h", NULL
		};
		execv(filename, __argv);
	}

	usleep(300);

	/* parent */

	open_process(&test_p, pid);
	process_attach(&test_p);

	list_for_each_entry(ivma, &test_p.vmas, node) {
		print_vma(ivma);
	}


#if 0
	list_for_each_entry(ivma, &test_p.vmas, node) {
		GElf_Ehdr *ehdr = (GElf_Ehdr*)ivma->content;
		if (check_elf_ehdr(ehdr) == -1) {
			continue;
		}
		fprintf(stderr, "%s is ELF.\n", ivma->name_);

		print_elf_ehdr(ehdr);

		print_elf_all_phdrs(ivma->content, ehdr);
		//print_elf_all_shdrs(buffer, ehdr);
	}
#endif
	printf("----- rbtree -------\n");
	rbtree_postorder_for_each_entry_safe(ivma, tmpvma, &test_p.vmas_rb, rb_node) {
		print_vma(ivma);
	}


	process_detach(&test_p);

	while (1) {
		sleep(1);
	}

	return 0;
}
