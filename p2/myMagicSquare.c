#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure that represents a magic square
typedef struct {
    	int size;           // dimension of the square
    	int **magic_square; // pointer to heap allocated magic square
} MagicSquare;

/* TODO:
 * Prompts the user for the magic square's size, reads it,
 * checks if it's an odd number >= 3 (if not display the required
 * error message and exit), and returns the valid number.
 */
int getSize() {
	int size;
	printf("Enter the size of the magic square (odd number >= 3)\n");
	scanf("%d", &size);

	// checks input size
	if (size < 3) {
		printf("Magic square size must be >= 3.\n");
		exit(1);
	}
	if (size % 2 == 0) {
		printf("Magic square size must be odd.\n");
		exit(1);
	}

    	return size;
}

/* TODO:
 * Makes a magic square of size n using the
 * Siamese magic square algorithm or alternate from assignment
 * or another valid alorithm that produces a magic square.
 *
 * n is the number of rows and columns
 *
 * returns a pointer to the completed MagicSquare struct.
 */
MagicSquare *generateMagicSquare(int n) {
	MagicSquare *ms = malloc(sizeof(MagicSquare));
	ms -> size = n;
	ms -> magic_square = calloc(ms -> size, sizeof(int*));
	for (int i = 0; i < n; i++) {
		*(ms -> magic_square + i) = calloc(ms -> size, sizeof(int));
	}

    	int i = 0;
	int j = n / 2;

 	// place the number 1 in the center
    	*(*(ms -> magic_square + i) + j) = 1;

	for (int k = 2; k <= n*n; k++) {
		// move diagonally up-right
		i = (i - 1 + n) % n;
		j = (j + 1) % n;

		// if the next position is already filled, move one row below
		if (*(*(ms -> magic_square + i) + j) != 0) {
			i = (i + 2) % n;
			j = (j - 1 + n) % n;
		}

		// place the next number in the magic square
		*(*(ms -> magic_square + i) + j) = k;
	}
    	return ms;
}

/* TODO:
 * Opens a new file (or overwrites the existing file)
 * and writes the square in the specified format.
 *
 * magic_square the magic square to write to a file
 * filename the name of the output file
 */
void fileOutputMagicSquare(MagicSquare *magic_square, char *filename) {
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		printf("Error: could not open file for writing.\n");
		exit(1);
	}
	
	int size = magic_square -> size;

	// write the size of the magic square to the file
	fprintf(file, "%d\n", size);

	// write the magic square to the file
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			fprintf(file, "%d", *(*(magic_square -> magic_square + i) + j));
			if (j != size - 1) {
				fprintf(file, ",");
  			}
		}
		fprintf(file, "\n");
	}

	fclose(file);
}


/* TODO:
 * Generates a magic square of the user specified size and
 * outputs the square to the output filename.
 *
 * Add description of required CLAs here
 * CLAs should be executable and name of output file
 */
int main(int argc, char **argv) {
// TODO: Check input arguments to get output filename
    	// check that we have correct CLAs
	if (argc != 2) {
		printf("Number of CLAs is not 2.\n");
		return 1;
	}
	char *file = *(argv + 1);

    	// TODO: Get magic square's size from user
	int size = getSize();
	
    	// TODO: Generate the magic square by correctly interpreting
    	//       the algorithm(s) in the write-up or by writing or your own.
    	//       You must confirm that your program produces a
    	//       Magic Sqare as described in the linked Wikipedia page.
	MagicSquare *ms = generateMagicSquare(size);

    	// TODO: Output the magic square
	fileOutputMagicSquare(ms, file);

	// free memory
	for (int i = 0; i < size; i++) {
		free(*(ms -> magic_square + i));
	}
	free(ms -> magic_square);
    	free(ms);

	return 0;
}
