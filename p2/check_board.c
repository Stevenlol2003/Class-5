#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *DELIM = ",";  // commas ',' are a common delimiter character for data strings

/*
 * Read the first line of input file to get the size of that board.
 *
 * PRE-CONDITION #1: file exists
 * PRE-CONDITION #2: first line of file contains valid non-zero integer value
 *
 * fptr: file pointer for the board's input file
 * size: a pointer to an int to store the size
 *
 * POST-CONDITION: the integer whos address is passed in as size (int *)
 * will now have the size (number of rows and cols) of the board being checked.
 */
void get_board_size(FILE *fptr, int *size) {
        char *line1 = NULL;
        size_t len = 0;

        if ( getline(&line1, &len, fptr) == -1 ) {
                printf("Error reading the input file.\n");
                free(line1);
                exit(1);
        }

        char *size_chars = NULL;
        size_chars = strtok(line1, DELIM);
        *size = atoi(size_chars);

        // free memory allocated for reading first link of file
        free(line1);
        line1 = NULL;
}



/* TODO:
 * Returns 1 if and only if the board is in a valid Sudoku board state.
 * Otherwise returns 0.
 *
 * A valid row or column contains only blanks or the digits 1-size,
 * with no duplicate digits, where size is the value 1 to 9.
 *
 * Note: p2A requires only that each row and each column are valid.
 *
 * board: heap allocated 2D array of integers
 * size:  number of rows and columns in the board
 */
int valid_board(int **board, int size) {
	// check rows first
	int *k;
	k = malloc(size*sizeof(int*));
	if ( k == NULL) {
		printf("Not enough memory.\n");
		exit(1);
	}

	for (int g = 0; g < size; g++ ) {
		*(k+g) = 0; // set all values in k to 0
      }

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			*(k+(*(*(board + i) + j))) += 1;
		}
		for (int u = 0; u < size; u++ ) {
			if (*(k+(*(*(board + i) + u))) > 1) {
				return 0;
			}
		}
		for (int g = 0; g < size; g++ ) {
			*(k+g) = 0; // set all values in k to 0
		}
	}


	// check coloums
	for (int a = 0; a < size; a++) {
		for (int b = 0; b < size; b++) {
			*(k+(*(*(board + b) +a))) += 1;
		}
		for (int l = 0; l < size; l++ ) {
			if (*(k+(*(*(board + l) + a))) > 1) {
				return 0;
			}
		}
		for (int r = 0; r < size; r++ ) {
			*(k+r) = 0; // set all values in k to 0
		}
	}
       return 1;
}



/* TODO: COMPLETE THE MAIN FUNCTION
 * This program prints "valid" (without quotes) if the input file contains
 * a valid state of a Sudoku puzzle board wrt to rows and columns only.
 *
 * A single CLA is required, which is the name of the file
 * that contains board data is required.
 *
 * argc: the number of command line args (CLAs)
 * argv: the CLA strings, includes the program name
 *
 * Returns 0 if able to correctly output valid or invalid.
 * Only exit with a non-zero result if unable to open and read the file given.
 */
int main( int argc, char **argv ) {

        // TODO: Check if number of command-line arguments is correct.
	   if (argc != 2) {
			printf("Number of CLAs is not 2.\n");
			exit(1);
	   }			
	   

        // Open the file and check if it opened successfully.
        FILE *fp = fopen(*(argv + 1), "r");
        if (fp == NULL) {
                printf("Can't open file for reading.\n");
                exit(1);
        }

        // Declare local variables.
        int size;

        // TODO: Call get_board_size to read first line of file as the board size.
	   get_board_size(fp, &size);

        // TODO: Dynamically allocate a 2D array for given board size. 
	   int **m;	
	   m = malloc(size*sizeof(int*));
	   if (m == NULL) {
			printf("Not enough memory.\n");
			exit(1);
	   }

        // Read the rest of the file line by line.
        // Tokenize each line wrt the delimiter character
        // and store the values in your 2D array.
        char *line = NULL;
        size_t len = 0;
        char *token = NULL;
        for (int i = 0; i < size; i++) {

                if (getline(&line, &len, fp) == -1) {
                        printf("Error while reading line %i of the file.\n", i+2);
                        exit(1);
                }

                token = strtok(line, DELIM);
                for (int j = 0; j < size; j++) {
                        // TODO: Complete the line of code below
                        // to initialize your 2D array.
                        *(*(m + i) +j) = atoi(token);
                        token = strtok(NULL, DELIM);
                }
        }

        // TODO: Call the function valid_board and print the appropriate
        //       output depending on the function's return value.
	  if (valid_board(m, size) == 1) {
		printf("valid\n");
	  } else {
		printf("invalid\n");
	  }	
		

        // TODO: Free all dynamically allocated memory.
	   free(*(m + 0));
	   free(*(m + 1));
	   free(*(m + 2));
	   free(*(m + 3));
	   free(*(m + 4));
	   free(*(m + 5));
	   free(*(m + 6));
	   free(*(m + 7));
	   free(*(m + 8));
	   free(m);

        //Close the file.
        if (fclose(fp) != 0) {
                printf("Error while closing the file.\n");
                exit(1);
        }

        return 0;
}
