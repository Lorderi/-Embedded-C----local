#include <stdio.h>

int main(void)
{
	float a = 0;

	for (;;) {
		printf("a = %f\n", a); //инкриментируем на 1 и обнаруживаем дырки(double точнее float`а)
		a = a + 1;	//остановилось 16777216.000000 (должно было на 26млн??)
	}

	return 0;
}