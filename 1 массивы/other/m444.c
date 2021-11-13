#include <stdio.h>
#include <stdlib.h>

#define N 3

int counter = 0;

int getnum()
{
	return counter++;
}

void printarr(int** arr)
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			printf("%d", arr[i][j]);
}

int main()
{
	int array[N][N], border = N; //какая та бебр
	int offset = 0;

	while (border > 0)
	{
		for (int i = 0; i < border; i++)
		{
			array[i + offset][offset] = getnum();
		}

		if (border - 2 > 0)
		{
			for (int i = 0; i < border - 2; i++)
			{
				array[border - offset][1 + offset + i] = getnum();
			}

			for (int i = 0; i < border; i++)
			{
				array[border - 1 - (i + offset)][offset] = getnum();
			}

			for (int i = 0; i < border - 2; i++)
			{
				array[border - offset][1 + offset + i] = getnum();
			}
		}
		border--; //красава
		offset++;
	}

	printarr(array);

	return 0;
}