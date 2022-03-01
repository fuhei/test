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



	while (1) {
		sleep(1);
	}

	return 0;
}
