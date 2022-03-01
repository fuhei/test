#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <gelf.h>
#include <sys/eventfd.h>

#define	ELFMAG		"\177ELF"
#define	SELFMAG		4
#define EI_NIDENT (16)

#define ET_NONE		0		/* No file type */
#define ET_REL		1		/* Relocatable file */
#define ET_EXEC		2		/* Executable file */
#define ET_DYN		3		/* Shared object file */
#define ET_CORE		4		/* Core file */
#define	ET_NUM		5		/* Number of defined types */
#define ET_LOOS		0xfe00		/* OS-specific range start */
#define ET_HIOS		0xfeff		/* OS-specific range end */
#define ET_LOPROC	0xff00		/* Processor-specific range start */
#define ET_HIPROC	0xffff		/* Processor-specific range end */

#define pr_err(fmt...) do { \
			fprintf(stderr, fmt); \
			exit(1); \
		} while(0)

const char *exename = "a.out";

struct uftrace_elf_iter {
	size_t i;
	size_t nr;

	union {
		GElf_Phdr phdr;
		GElf_Shdr shdr;
		GElf_Nhdr nhdr;
		GElf_Sym  sym;
		GElf_Dyn  dyn;
		GElf_Rel  rel;
		GElf_Rela rela;
	};

	void	*note_name;
	void	*note_desc;

	/* hidden */
	int      type;
	int      ent_size;
	char     *strtab;
	char     *data;
};

#define elf_for_each_phdr(ehdr, iter)					\
	for ((iter)->i = 0, (iter)->nr = ehdr.e_phnum;		\
	     (iter)->i < (iter)->nr &&					\
		     memcpy(&(iter)->phdr,				\
			    file_map + ehdr.e_phoff +	\
			    (iter)->i * ehdr.e_phentsize,	\
			    ehdr.e_phentsize);			\
	     (iter)->i++)

int main(int argc, char *argv[])
{
	int fd;
	char elf_ident[EI_NIDENT];
	uint16_t e_type;
	uint16_t e_machine;

	struct stat stbuf;
	void *file_map = NULL;
	GElf_Ehdr ehdr;
	struct uftrace_elf_iter iter;

	int pid;
	int ready;
	int ret = -1;

	fd = open(exename, O_RDONLY);

	if (read(fd, elf_ident, sizeof(elf_ident)) < 0)
		pr_err("Cannot read '%s'", exename);

	if (memcmp(elf_ident, ELFMAG, SELFMAG))
		pr_err("not elf '%s'", exename);

	if (read(fd, &e_type, sizeof(e_type)) < 0)
		pr_err("Cannot read '%s'", exename);

	if (e_type != ET_EXEC && e_type != ET_DYN)
		pr_err("Type wrong %s", exename);

	if (read(fd, &e_machine, sizeof(e_machine)) < 0)
		pr_err("Cannot read '%s'", exename);

	fstat(fd, &stbuf);

	file_map = mmap(NULL, stbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	memcpy(&ehdr, file_map, sizeof(ehdr));

	elf_for_each_phdr(ehdr, &iter) {
		if (iter.phdr.p_type == PT_DYNAMIC) {
			break;
		}
	}

	ready = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE);
	if (ready < 0)
		pr_err("creating eventfd failed: %d\n", ready);

	pid = fork();
	if (pid < 0)
		pr_err("cannot start child process");

	if (pid == 0) {
		
		return ret;
	}

	munmap(file_map, stbuf.st_size);
	file_map = NULL;

	close(fd);

	return 1;
}
