#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <gelf.h>
#include <string.h>
#include <stdlib.h>

const char *filename = "a.out";

int main()
{
	const char *trace_funcs[] = {
		"__cyg_profile_func_enter",
		"__fentry__",
		"mcount",
		"_mcount",
		"__gnu_mcount_nc",
	};
	char *name;
	unsigned i;
	struct stat stbuf;
	GElf_Ehdr ehdr;

	/* 1. Initial */
	int fd = open(filename, O_RDONLY);
	fstat(fd, &stbuf);
	void *file_map = mmap(NULL, stbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	memcpy(&ehdr, file_map, sizeof(ehdr));

	/* 2. Check ELF format */
	//TODO: Add check ELF
	
	/*  */
	GElf_Shdr *shdr = file_map + ehdr.e_shoff;
	int nr_shnum = ehdr.e_shnum;
	GElf_Shdr i_shdr;
	int ent_size = 0;
	char *data;

	for (i = 0; i < ehdr.e_shnum; i++) {
		memcpy(&i_shdr, file_map + ehdr.e_shoff + i * ehdr.e_shentsize, ehdr.e_shentsize);
		if (i_shdr.sh_type == SHT_DYNSYM) {
			printf("Find dynamic symbols.\n");
			ent_size = i_shdr.sh_entsize;
			data = file_map + i_shdr.sh_offset;
			break;
		}
	}

	if (i_shdr.sh_type != SHT_DYNSYM) {
		fprintf(stderr, "cannot find dynamic symbols.. skipping\n");
		exit(1);
	}
	
	int type = i_shdr.sh_type;
	GElf_Sym  sym;
	char *i_strtab = file_map + shdr[i_shdr.sh_link].sh_offset;

	for (i = 0; i < i_shdr.sh_size / ent_size && type == SHT_DYNSYM; i++) {
		memcpy(&sym, &data[i * ent_size], ent_size);

		char *name = (char*)(i_strtab + sym.st_name);

		if (GELF_ST_TYPE(sym.st_info) != STT_FUNC &&
			GELF_ST_TYPE(sym.st_info) != STT_GNU_IFUNC) {
			continue;
		}

		printf("Name = %s\n", name);
	}
	
}
