#include <dlfcn.h>
#include "func.h"
#include <stdio.h>
//void *dlopen(const char *filename, int flags);
//int dlclose(void *handle);
//void *dlsym(void *handle, const char *symbol);
//char *dlerror(void);

int main(void) {

	void *library_handler;
	float (*minus)(float, float), (*plus)(float, float), (*um)(float, float), (*del)(float, float); //объявление указателя на функцию принимающую 2 параметра
	char *error;

	library_handler = dlopen("libfunc.so", RTLD_LAZY);
	if (!library_handler){
		fprintf(stderr,"dlopen() error: %s\n", dlerror());
		return 1;
	}

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
				plus = dlsym(library_handler, "func_plus");
				if ((error = dlerror()) != NULL)  {
					fprintf (stderr, "dlsym() error: %s\n", error);
					return 1;
				}
				result = plus(num1, num2);
			}
			if (table == 2) {
				minus = dlsym(library_handler, "func_minus");
				if ((error = dlerror()) != NULL)  {
					fprintf (stderr, "dlsym() error: %s\n", error);
					return 1;
				}
				result = minus(num1, num2);
			}
			if (table == 3) {
				um = dlsym(library_handler, "func_um");
				if ((error = dlerror()) != NULL)  {
					fprintf (stderr, "dlsym() error: %s\n", error);
					return 1;
				}
				result = um(num1, num2);
			}
			if (table == 4) {
				del = dlsym(library_handler, "func_del");
				if ((error = dlerror()) != NULL)  {
					fprintf (stderr, "dlsym() error: %s\n", error);
					return 1;
				}
				result = del(num1, num2);
			}
			printf("Полученный результат : %g\n", result);
		}
		else
			if (table != 5) {
				printf("Неверный ввод\n");
			}
			else break;

	}
	dlclose(library_handler);
	return 0;
}
