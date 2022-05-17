#include <stdio.h>
#define N 7

int num = 1;

int plus() {
	return num++;
}


int main(void)
{

	int matrix[N][N], i = 0, j = 0, nomm = 0;

	while (num <= (N * N)){
		i = nomm;
		for (j = nomm; j < N - nomm; j++) {
			matrix[i][j] = plus();
		}
		j = N - nomm - 1;
		for (i = nomm + 1; i < N - nomm - 1; i++) {
			matrix[i][j] = plus();
		}
		if (num > (N * N)) {
			break;
		}
		i = N - nomm - 1;
		for (j = N - nomm - 1; j >= nomm; j--) {
			matrix[i][j] = plus();
		}
		j = nomm;
		for (i = N - nomm - 2; i > nomm; i--) {
			matrix[i][j] = plus();
		}
		nomm++;
	}
	

	printf("Выходящий массив\n\n");
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}



	return 0;
}
