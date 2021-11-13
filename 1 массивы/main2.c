#include <stdio.h>
#define N 3

//Вывести заданный массив размером N в обратном порядке
int main(void)
{
	char rez,matrix[N][N] = {{1,2,3},
			 	  {4,5,6},
				  {7,8,9}};
	int i, n, sch = 1, x = N - 1, high = (N*N)/2;
	
	printf("Входящий массив\n\n");
	for (i = 0;i < N; i++){
		for (n = 0;n < N; n++){
			printf("%d ", matrix[i][n]);
		}
		printf("\n");
	}
	
	for (i = 0;i < N; i++){
		for(n = 0;n < N; n++){
			if (sch > high){
				break;
			}
			rez = matrix[i][n];
			matrix[i][n] = matrix[x-i][x-n];
			matrix[x-i][x-n] = rez;
			sch++;
		}
	}
	
	printf("Выходящий массив\n\n");
	for (i = 0;i < N; i++){
		for (n = 0;n < N; n++){
			printf("%d ", matrix[i][n]);
		}
		printf("\n");
	}
	return 0;
}