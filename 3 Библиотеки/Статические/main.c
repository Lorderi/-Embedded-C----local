#include "func.h"
#include <stdio.h>

int main(void) {
	int table;
	float num1, num2, result=0;

	for (;;) {
		printf("\n||Меню Калькулятора||\n||1-сложить||\n||2-вычесть||\n||3-умножить||\n||4-поделить||\n||5-выйти||\n");
		scanf("%d", &table);
	
		if ((table >= 0) && (table < 5)) {

			printf("Введите число 1: ");
			scanf("%f", &num1);
			printf("Введите число 2: ");
			scanf("%f", &num2);

			if (table == 1) {
				result = func_plus(num1, num2);
			}
			if (table == 2) {
				result = func_minus(num1, num2);
			}
			if (table == 3) {
				result = func_um(num1, num2);
			}
			if (table == 4) {
				result = func_del(num1, num2);
			}
			printf("Полученный результат : %g\n", result);
		}
		else
			if (table != 5) {
				printf("Неверный ввод\n");
			}
			else break;

	}
	return 0;
}
