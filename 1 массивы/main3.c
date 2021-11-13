#include <stdio.h>
#define N 4

//Вывести заданный массив размером N в обратном порядке
//Простите за goto
int main(void)
{
	char rez,matrix[N][N];
	int i,n;
	
	
	for (i = 0;i < N; i++){
		for(n = 0;n < N; n++){
			for(;;){
				if (n < 2){
					if (i < 2){
							matrix[i][n]=0;
						}
					else{
						goto ex;
					}
					}
				else{
					ex:
					matrix[i][n]=1;
				}
		
				break;
				    }
		}
	}
	matrix[1][1]=1;
	printf("Выходящий массив\n\n");
	for (i = 0;i < N; i++){
		for (n = 0;n < N; n++){
			printf("%d ", matrix[i][n]);
		}
		printf("\n");
	}
	return 0;
}