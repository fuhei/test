#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#include <assert.h>
#include <gelf.h>

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE*)0)->MEMBER)
#endif

const char *filename = "a.out";
pid_t pid;

void sig_handler(int signum)
{
	kill(pid, SIGINT);
	exit(1);
}

/**
 * @brief print GElf_Ehdr
 * 
 * @param ehdr 
 */
void print_elf_ehdr(GElf_Ehdr *ehdr)
{
	printf("%20s = %-6s\n", "e_ident", ehdr->e_ident);

	printf("%20s = %6d  ", "e_type", ehdr->e_type);
	switch (ehdr->e_type) {
	case ET_NONE:
		printf("No file type \n");
		break;
	case ET_REL:
		printf("Relocatable file \n");
		break;
	case ET_EXEC:
		printf("Executable file \n");
		break;
	case ET_DYN:
		printf("Shared object file \n");
		break;
	case ET_CORE:
		printf("Core file \n");
		break;
	case ET_NUM:
		printf("Number of defined types \n");
		break;
	case ET_LOOS:
		printf("OS-specific range start \n");
		break;
	case ET_HIOS:
		printf("OS-specific range end \n");
		break;
	case ET_LOPROC:
		printf("Processor-specific range start \n");
		break;
	case ET_HIPROC:
		printf("Processor-specific range end \n");
		break;
	}

	fflush(stdout);

	printf("%20s = %6d  ", "e_machine", ehdr->e_machine);
	switch (ehdr->e_machine) {
	case EM_NONE:
		printf("No machine type \n");
		break;
	case EM_ARM:
		printf("Arm\n");
		break;
	case EM_X86_64:
		printf("X86_64\n");
		break;
	case EM_M32 ... EM_RCE:
	case EM_FAKE_ALPHA ... EM_TINYJ:
	case EM_PDSP ... EM_NUM:
		printf("Not support\n");
		break;
	}

	fflush(stdout);
#define PRINT_I(e)	do { printf("%20s = %6d\n", #e, ehdr->e); fflush(stdout); } while(0)
#define PRINT_L(e)	do { printf("%20s = %6ld\n", #e, ehdr->e); fflush(stdout); } while(0)
	PRINT_I(e_version);
	PRINT_L(e_entry);
	PRINT_L(e_phoff);
	PRINT_L(e_shoff);
	PRINT_I(e_flags);
	PRINT_I(e_ehsize);
	PRINT_I(e_phentsize);
	PRINT_I(e_phnum);
	PRINT_I(e_shentsize);
	PRINT_I(e_shnum);
	PRINT_I(e_shstrndx);
#undef PRINT_I
#undef PRINT_L
}

/**
 * @brief main
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[])
{
	signal(SIGINT, sig_handler);

	pid = fork();
	if (pid == 0) { //Child
		execv(filename, NULL);
	}

	sleep(1);
	/* parent */

	char proc_child_mem[] = "/proc/1234567890/mem";
	snprintf(proc_child_mem, sizeof(proc_child_mem), "/proc/%d/mem", pid);
	int memfd = open(proc_child_mem, O_RDWR);
	if (memfd <= 0) {
		fprintf(stderr, "open %s failed.\n", proc_child_mem);
		kill(pid, SIGINT);
		exit(1);
	}

	char proc_child_maps[] = "/proc/1234567890/maps";
	snprintf(proc_child_maps, sizeof(proc_child_maps), "/proc/%d/maps", pid);
	int mapsfd = open(proc_child_maps, O_RDONLY);
	if (mapsfd <= 0) {
		fprintf(stderr, "open %s failed.\n", proc_child_maps);
		kill(pid, SIGINT);
		exit(1);
	}

	/**
	 * /proc/PID/maps
	 * 56028a405000-56028a433000 r--p 00000000 fd:00 70252                      /usr/bin/bash
	 */
	lseek(mapsfd, 0, SEEK_SET);
	FILE *mapsfp = fdopen(mapsfd, "r");
	if (mapsfp == NULL) {
		fprintf(stderr, "fdopen %d failed.\n", mapsfd);
		kill(pid, SIGINT);
		exit(1);
	}

	long rv;
	rv = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
	if (rv != 0) {
		fprintf(stderr, "Attach failed.\n");
		kill(pid, SIGINT);
		exit(1);
	}

	/**
	 * @brief for all vma
	 * 
	 */
	do {
		unsigned long start, end, offset;
		unsigned int maj, min, inode;
		char perms[5], name_[256], *name = name_;
		int r;
		char line[1024];

		if (!fgets(line, sizeof(line), mapsfp))
			break;

		r = sscanf(line, "%lx-%lx %s %lx %x:%x %d %255s",
			   &start, &end, perms, &offset,
			   &maj, &min, &inode, name_);

#if 0
		printf("%16lx-%16lx %6s %8lx %4x:%4x %8d %s\n",
			   start, end, perms, offset,
			   maj, min, inode, name_);
#endif
		char *buffer = malloc(end - start + 1);
		assert(buffer && "Malloc buffer failed");

		r = pread(memfd, buffer, end - start + 1, (off_t)start);
		if (r <= 0) {
			//fprintf(stderr, "pread failed in %s.\n", name_);
			continue;
		}

		/**
		 * @brief Handler ELF VMA
		 * 
		 */
		GElf_Ehdr *ehdr = (GElf_Ehdr*)buffer;
		if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
			ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
			ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
			ehdr->e_ident[EI_MAG3] != ELFMAG3) {
			//fprintf(stderr, "%s is not ELF.\n", name_);
			continue;
		} else {
			fprintf(stderr, "%s is ELF.\n", name_);
		}

		print_elf_ehdr(ehdr);

		free(buffer);
	} while (1);

	/**
	 * @brief Detach child process
	 * 
	 */
	sleep(1);
	fprintf(stdout, "Detach.\n");
	rv = ptrace(PTRACE_DETACH, pid, NULL, NULL);
	if (rv != 0) {
		fprintf(stderr, "Detach failed.\n");
		kill(pid, SIGINT);
		exit(1);
	}

	while (1) {
		sleep(1);
	}

	return 0;
}
