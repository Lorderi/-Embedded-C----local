#include <stdio.h>
#define N 3

//Вывести квадратную матрицу по заданному N
//Хз как динамич.матрицу сделать, а const Вы сказали не надо определять границы
int main(void)
{
	int matrix[N][N], i, n;
	
	printf("Ввод массива\n\n");
	for (i = 0;i < N; i++){
		n=i+1;
		printf("Cтрока %d:\n\n",n);
		for (n = 0;n < N; n++){
			scanf("%d", &matrix[i][n]);
		}
		printf("\n");
	}
	
	printf("Вывод массива\n\n");
	for (i = 0;i < N; i++){
		for (n = 0;n < N; n++){
			printf("%d ", matrix[i][n]);
		}
		printf("\n");
	}
	
	return 0;
}