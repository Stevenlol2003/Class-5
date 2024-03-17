#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(int argc, char *argv[]) {
	// check number of CLAs
	if (argc != 3) {
        printf("Usage: sendsig <signal type> <pid>\n");
        return 1;
	}

	// get signal type (-i for SIGINT or -u for SIGUSR1) 
	int signum;
	if (strcmp("-i", argv[1]) == 0) {
		signum = SIGINT;
	} else if (strcmp("-u", argv[1]) == 0) {
		signum = SIGUSR1;
	} else {
		printf("Invalid signal.\n");
		return 1;
	}

	// get pid
	pid_t pid = atoi(argv[2]);

	// send signal
	if (kill(pid, signum) == -1) {
		perror("Error sending signal\n");
	return 1;
	}

	return 0;
}
