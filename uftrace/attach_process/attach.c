#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

const char *filename = "a.out";
pid_t pid;

void sig_handler(int signum)
{
	kill(pid, SIGINT);
	exit(1);
}

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

		printf("%16lx-%16lx %6s %8lx %4x:%4x %8d %s\n",
			   start, end, perms, offset,
			   maj, min, inode, name_);

		char buffer[2048];
		r = pread(memfd, buffer, 8, (off_t)start);
		if (r <= 0) {
			perror("pread failed.\n");
			kill(pid, SIGINT);
			exit(1);
		}
	} while (1);


	while (1) {
		sleep(1);
	}

	return 0;
}
