#include <stdio.h>

#define ROWS 128
#define COLS 8

int arr2D[ROWS][COLS];

int main() {
	for (int k = 0; k < 100; k++) {
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLS; j++) {
				arr2D[i][j] = k + i + j;
			}
		}
	}

	return 0;
}
