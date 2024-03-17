#include <stdio.h>

#define ROWS 3000
#define COLS 500

int arr2D[ROWS][COLS];

int main() {
	for (int j = 0; j < COLS; j++) {
		for (int i = 0; i < ROWS; i++) {
			arr2D[i][j] = i + j;
		}
	}

	return 0;
}
