#include <stdio.h>
#define N 6

//Заполнить матрицу числами от 1 до N^2
int main(void)
{
	int n_schn=0,n=0,i=0, highborder=N-1, matrix[N][N];
	while(sch < ){
		
		if ((sch%2)==0){
		
			if ((n_sch%2)==0){
				while(n <= highborder){ //n=0..1..2..  border=5,4,3 N-1-n    1 5 9
					scanf("%d",&matrix[i][n]); //i=0,1,2
				}
			}
			else{
				while(n >= highborder){ //n=4..3..2..N-2-n  border=0,1,2 n     3 7 11
					scanf("%d",&matrix[i][n]); //i=5,4,3 N-1-i
				}
			}
			n_sch++;
		}
		else{
			if ((i_sch%2)==0){
				while(i <= highborder){ //i=1..1..2..  border=5,4,3  N-1-n   2 6 10 
					scanf("%d",&matrix[i][n]); //n=5,4,3 N-1-n
				}
			}
			else{
				while(i >= highborder){ //i=4..3..N-2-i  border=1,2  i+1        4 8
					scanf("%d",&matrix[i][n]); //n=0,1 n
				}
			}
			i_sch++;
		
		i++;n++;
		
		}
		sch++;
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