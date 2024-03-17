#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int count = 0;

// signal handler for SIGALRM
void handler_SIGALRM(int sig) {
	int pid = getpid();
	time_t t;


	// time() and check return value
	time(&t);
    	if(t == -1) {
		printf("Error with time()\n");
		exit(1);
	}

	// ctime() and check return value
	char* time = ctime(&t);
	if (time == NULL) {
		printf("Error with ctime()\n");
		exit(1);
	}
	printf("PID: %d CURRENT TIME: %s", pid, time);

	alarm(4);
}

// signal handler for SIGINT (ctrl + c)
void handler_SIGINT(int sig) {
	printf("\nSIGINT handled.\n");
	printf("SIGUSR1 was handled %d times. Exiting now.\n", count);
	exit(0);
}

// signal handler for SIGUSR1
void handler_SIGUSR1(int sig) {
	count++;

	printf("SIGUSR1 handled and counted!\n");
}


int main() {
	printf("PID and time print every 4 seconds.\n");
	printf("Type Ctrl-C to end the program.\n");

	// initialize the sigaction struct via memset() so that it is cleared 
	// (i.e, zeroed out) before used
	struct sigaction act;
	memset(&act, 0, sizeof(act));

	act.sa_handler = handler_SIGALRM;
	if (sigaction(SIGALRM, &act, NULL) != 0) {
		printf("Error binding SIGALRM handler\n");
		exit(1);
	}

	act.sa_handler = handler_SIGINT;
	if (sigaction(SIGINT, &act, NULL) != 0) {
		printf("Error binding SIGINT handler\n");
		exit(1);
	}

	act.sa_handler = handler_SIGUSR1;
	if (sigaction(SIGUSR1, &act, NULL) != 0) {
		printf("Error binding SIGAUSR1 handler\n");
		exit(1);
	}
	
	alarm(4);

	while (1) {
		
	}

    return 0;
}

