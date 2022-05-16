//решение без подсчета количества аргументов у функций, они всегда строго заданны: 2 float`а

#include <dlfcn.h>
#include "func.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>


#define NUM_OF_PLUG 25 //переменная для предсоздания указателей на плагины, т.е. максимум плагинов


int main() {

	//указатели на работу с папками
	DIR *dir;
    struct dirent *entry;
	//опенер для поиска плагинов
    dir = opendir("./plugin");
    if (!dir) {
        perror("diropen");
        exit(1);
    };

	float (*mass_of_func[NUM_OF_PLUG])(float, float); //массив указателей на функции
	char menu[256] = "\nМеню\n\0";

	/*
		для наглядности только у нас так будет с функциями
		char days[][12] = {
			"Понедельник", "Вторник", "Среда", "Четверг",
			"Пятница", "Суббота", "Воскресенье"
		};
	*/
	char fin_funcs[NUM_OF_PLUG][64] = {0}; //создаем массив строк чтобы потом обращаться к функциям
	char fin_libs[NUM_OF_PLUG][64] = {0}; //создаем массив строк чтобы потом обращаться к функциям

	char tmpbuff[512] = {0}; //временной буффер
	char namelib[] ="libfunc_"; //ключевое слово для поиска либ
	int quanity = 0; //подсчет кол-ва найденных имен
	char * func; //для разделителя strtok и strchr 
	
	//обработчик если плагины были не найдены
	if ((entry = readdir(dir)) == NULL){
		printf("Плагины не найдены\n");
	}
	else{
		//добавление имен плагинов в общий буффер
		while ( (entry = readdir(dir)) != NULL) {
			if ( strncmp(entry -> d_name , namelib, strlen(namelib) ) == 0 ){ //сравниваем с ключевым словом
				//заполнение fin_libs
				strcpy(&fin_libs[quanity][0], entry -> d_name);	// заполняем массив строк с именами либ
				
				//заполнение fin_funcs
				func = strtok (entry -> d_name,"."); //отделяем часть .so получаем libfunc_minus
				strcpy(tmpbuff, func); //во временный буффер добавляем libfunc_minus
				func = strchr(tmpbuff, 'f'); //находим первое вхождение f (func_minus)
				strcpy(&fin_funcs[quanity][0], func);	// заполняем массив строк с именами функций
				quanity++;
			}
    	}
	}
    closedir(dir); //закрытие указателя

	//указатели на ленивое связывание и обработчик ошибок
	void *library_handler[NUM_OF_PLUG];
	char *error;

	int i = 0;
	
	//заполняем указатели на либы, то есть связываем либы к процессу
	//STRTOK не может одновременно в двух циклах работать поэтому потом добавим еще один
	printf("Подключаемые библиотеки:\n");
	for (i = 0; i < quanity; i++){
				
		//Блок с заполнением указателей на либы
		strcpy(tmpbuff, "./plugin/"); //создаем строку /plugin/libfunc_del.so
		strcat(tmpbuff, &fin_libs[i][0]);
		printf("%s\n", tmpbuff);
		library_handler[i] = dlopen(tmpbuff, RTLD_LAZY);
		if (!library_handler){
			fprintf(stderr,"dlopen() error: %s\n", dlerror());
			return 1;
		}

		//Блок с заполнением меню
		sprintf (tmpbuff, "%d - %s\n", i + 1, &fin_funcs[i][0]);
		strcat(menu, tmpbuff);

	}
	printf("\n");
	sprintf (tmpbuff, "%d - выход\n\n", quanity + 1);
	strcat(menu, tmpbuff);
	

	int table;
	float num1, num2, result = 0;

	printf("Подключаемые функции:\n");
	for (i = 0; i < quanity; i++){
		printf("%s\n", &fin_funcs[i][0]);
	}
	
	for (;;) {

		printf("%s\n", menu);
		scanf("%d", &table);
	
		if ((table >= 0) && (table < quanity + 1)) {

			printf("Введите число 1: ");
			scanf("%f", &num1);
			printf("Введите число 2: ");
			scanf("%f", &num2);

			for (i = 0; i < quanity; i++){
				if (table == (i + 1)){
					/*
					пример как было:
					plus = dlsym(library_handler, "func_plus");
					if ((error = dlerror()) != NULL)  {
						fprintf (stderr, "dlsym() error: %s\n", error);
						return 1;
					}
					result = plus(num1, num2);*/
					sprintf (tmpbuff, "%s", &fin_funcs[i][0]); //берем строку из массива строк с функциями
					mass_of_func[i] = dlsym(library_handler[i], tmpbuff);
					if ((error = dlerror()) != NULL)  {
						fprintf (stderr, "dlsym() error: %s\n", error);
						return 1;
					}
					result = mass_of_func[i](num1, num2);
					printf("Полученный результат : %g\n", result);
				}
				
			}
		}
		/*обработчик ошибки*/
		if ( result == 0 ){
				if (table != (quanity + 1)) {
					printf("Неверный ввод\n");
				}
				else {	break;	}
		}
	
		result = 0;
	}

	dlclose(library_handler);
	return 0;

}
