#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

int count = 0;

// signal handler for SIGFPE when division of 0 happens
void handler_SIGFPE(int sig) {
	printf("Error: a division by 0 operation was attempted.\n");
	printf("Total number of operations completed successfully: %d\n", count);
	printf("The program will be terminated.\n");
	exit(0);
}

// signal handler for SIGINT (ctrl + c)
void handler_SIGINT(int sig) {
	printf("\nTotal number of operations completed successfully: %d\n", count);
	printf("The program will be terminated.\n");
	exit(0);
}

int main() {
	// initialize the sigaction struct via memset() so that it is cleared 
	// (i.e, zeroed out) before you used
	struct sigaction act;
	memset(&act, 0, sizeof(act));

	act.sa_handler = handler_SIGFPE;
	if (sigaction(SIGFPE, &act, NULL) != 0) {
		printf("Error binding SIGFPEhandler\n");
		exit(1);
	}

	act.sa_handler = handler_SIGINT;
	if (sigaction(SIGINT, &act, NULL) != 0) {
		printf("Error binding SIGINT handler\n");
		exit(1);
	}

	while (1) {
		char buffer[100]; // buffer
		int int1; // first integer
		printf("Enter first integer: ");
 		// get input
		char* input = fgets(buffer, 100, stdin);
		if (input == NULL) {
			return 1;
		}
		else {
			int1 = atoi(buffer); // converts strings to int just incase
		}

		int int2; // second integer
		printf("Enter second integer: ");
 		// get input
		input = fgets(buffer, 100, stdin);
		if (input == NULL) {
			return 1;
		}
		else {
			int2 = atoi(buffer); // converts strings to int just incase
		}

		int quotient = int1 / int2; // calculate quotient
		int remainder = int1 % int2; // calculate remainder

		// print results
		printf("%d / %d is %d with a remainder of %d\n", int1, int2, quotient, remainder);

		count++;
	}
    return 0;
}

