/*
УСЛОВИЯ:
магазин - кол-во товаров.
1000 5 шт

зайти в свободный магазин(если зашел занят)
то есть реализовать свободный заход в разные магазины разных потоков
а в один нельзя

покупатель заходит, параметр - потребность 10 000
перебирает по очереди свободный магазин.
скупает выходит и засыпает на 2 сек

погрузчик - докидывает 1000
засыпает

Выводить лог информации

*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
#include <pthread.h>
#define NUM_OF_MAG 5
#define NUM_OF_POK 3
#define POK_VALUE 1000
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)
/*#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define BAD_MESSAGE         -13
#define SUCCESS               0  */    

static int magazine[NUM_OF_MAG] = {1000, 1000, 1000, 1000, 1000};
static int potrebnost[NUM_OF_POK] = {10000, 10000, 10000};
pthread_mutex_t mutex;

/*ПОЛУЧИЛАСЬ ИМИТАЦИЯ ЗАНЯТОСТИ МАГАЗИНА, Т.К. МЬЮТЕКСЫ ЗАХВАТЫВАЮТ КРИТИЧЕСКУЮ СЕКЦИЮ, А НЕ ГЛОБЗНАЧ
 *МАГАЗИНА. МЬЮТЕКС ЗАМЫКАЕТСЯ КОГДА КТО-ТО ЗАХОДИТ В КАКОЙ-ТО!! МАГАЗИН, А НАДО ЧТОБЫ ИМЕННО ГЛОБЗНАЧ 
 *ОДНОГО МАГАЗИНА БЫЛ ДОСТУПЕН ОДНОМУ ПОТОКУ. А ПОЛУЧАЕТСЯ ЧТО ЕСЛИ МЬЮТЕКС ЗАНЯТ, А НЕ МАГАЗИН, ТО 
 *ПЕРЕХОДИМ В ДР МАГАЗИН. ИЗ-ЗА ЭТОГО РАБОТА МЕДЛЕННЕЕ И ВЫПОЛНЯЕТСЯ БОЛЕЕ ПОСЛЕДОВАТЕЛЬНО
 */
 
//1 решить проблему с id: mutex должен быть общий а id нет
//то есть структура должна быть и общая и нет
//2 как провереть заблочено мьютексом или нет?
//UDP: решено

//!!!!!!!!!!решить проблему одновременного использования массива магазина

//Структуры для передачи id в потоки
//Покупатель
typedef struct pok_tag {
	int id_p;
//UDP:НЕ НАДО мьютекс в структуру чтобы передавать потокам
//	pthread_mutex_t mutex;
} pokarg_t;
//Грузчик
typedef struct gru_tag {
	int id_d;
//	pthread_mutex_t mutex;
} gruarg_t;


//функция ПОКУПАТЕЛЬ
//НУЖНО вычесть из какого то магазина 1000, вычесть из СВОЕЙ потребности 1000
void* buy(void *args) {
	int local_mag; //локальное значение магазина 
	pokarg_t *arg = (pokarg_t*)args; //переданные аргументы в функцию
	int local_potr = potrebnost[arg->id_p - 1]; //забираем потребность из общего в локальную функцию и работаем с ним
	
	
	
	//пока потребность имеется
	while (local_potr > 0) { 
		//перебираем магазины
		for (int i = 0; i < NUM_OF_MAG; i++) {
			//Если в магазине есть столько сколько надо покупателю
			if (magazine[i] >= POK_VALUE){
				printf("Покупатель поток №%d: в магазине %d есть еда, пробую зайти\n", arg->id_p, i + 1);
				//блокировка критической секции
				if (pthread_mutex_trylock(&mutex) == 0) {
					local_mag = magazine[i]; //инициализируем глобзнач магазина в локал
					local_mag = local_mag - POK_VALUE; //вычитаем (покупаем) из магаза
					local_potr = local_potr - POK_VALUE; //уменьшаем потребность
					magazine[i] = local_mag; //инициализируем локал магазина в глобал
					printf("Покупатель поток №%d: в магазин %d зашел, взял еду. Потребность: %d\n", arg->id_p, i + 1, local_potr);
					pthread_mutex_unlock(&mutex);
					printf("Покупатель поток №%d: сплю 2 сек\n", arg->id_p);
					sleep(2);
				}  //разблокировка кс
				else {
					printf("Покупатель поток №%d: магазин %d занят(mutex!!), иду в другой\n", arg->id_p, i + 1);
				}
			}
			else {
				printf("Покупатель поток №%d: в магазине %d нет еды, иду в другой\n", arg->id_p, i + 1);
			}
			if (local_potr <= 0){break;}
		}	
	}
	printf("Покупатель поток №%d завершается. Потребность %d: \n", arg->id_p, local_potr);
	potrebnost[arg->id_p - 1] = local_potr;

	return 0;
}

//функция ГРУЗЧИК
//НУЖНО прибавить в какой-то магазин 1000
void* give(void *args) {
	int local_mag;
	gruarg_t *arg = (gruarg_t*)args;
	int local_potr_1, local_potr_2, local_potr_3;
	local_potr_1 = potrebnost[0];
	local_potr_2 = potrebnost[1];
	local_potr_3 = potrebnost[2];
	
	while ((local_potr_1 > 0) || (local_potr_2 > 0) || (local_potr_3 > 0) ) {
		for (int i = 0; i < NUM_OF_MAG; i++) {
			//Если в магазине нет столько сколько надо покупателю
			if (magazine[i] < POK_VALUE){
				printf("Грузчик поток №%d: в магазине %d нет еды, пробую загрузить\n", arg->id_d, i + 1);
				//блокировка критической секции
				if (pthread_mutex_trylock(&mutex) == 0) {
					local_mag = magazine[i]; //инициализируем глобзнач магазина в локал
					local_mag = local_mag + POK_VALUE; //прибавляем (грузим) в магаз
					magazine[i] = local_mag; //инициализируем локал магазина в глобал
					printf("Грузчик поток №%d: в магазин %d зашел, загрузил еду. В магазине: %d\n", arg->id_d, i + 1, magazine[i]);
					pthread_mutex_unlock(&mutex); //разблокировка кс
				} 
				else {
					printf("Грузчик поток №%d: магазин %d занят(mutex!!), иду в другой\n", arg->id_d, i + 1);
				}
				printf("Грузчик поток №%d: сплю 2 сек\n", arg->id_d);
				sleep(2);
			}
			else {
				printf("Грузчик поток №%d: в магазине %d есть еда, иду в другой. В магазине: %d\n", arg->id_d, i + 1, magazine[i]);
			}
		}
		local_potr_1 = potrebnost[0];
		local_potr_2 = potrebnost[1];
		local_potr_3 = potrebnost[2];
		printf("potrebnost[0]:%d; potrebnost[1]:%d; potrebnost[2]:%d\n", local_potr_1, local_potr_2, local_potr_3);
	
	}
	
	
//	options_t *arg = (options_t*)args;
//	pthread_mutex_lock(&mutex);
//	local = counter;
//	printf("pls %d\n", counter);
//	local = local + 1;
//	counter = local;
//	pthread_mutex_unlock(&mutex);
	
	printf("Грузчик поток №%d завершается, т.к. потребности нет.\n", arg->id_d);
	return 0;
}

#define NUM_OF_POKTHREADS 3
#define NUM_OF_GRUTHREADS 3

int main() {
	pthread_t pok_threads[NUM_OF_POKTHREADS], gru_threads[NUM_OF_GRUTHREADS]; //pthread - покупатели gthread - грузчики// указатель на поток, id потока
	int i; //счетчик
	int status1, status2, status_addr1, status_addr2;
	pokarg_t p_param[NUM_OF_POKTHREADS]; //параметры покупателей
	gruarg_t g_param[NUM_OF_GRUTHREADS]; //параметры грузчиков
	

//	printf("counter = %d\n", counter);
	//инициализация мьютекса
	if ((pthread_mutex_init(&mutex, NULL)) != 0){
		handle_error("mutex_init");
	}
	//инициализация в структуру для передачи в потоки id потоков
	for (i = 0; i < NUM_OF_POKTHREADS; i++) {
       	 p_param[i].id_p = i + 1; //id покупателей от 1 до 3 
        }
        
        for (i = 0; i < NUM_OF_GRUTHREADS; i++) {
       	 g_param[i].id_d = i + 1; //id грузчиков от 1 до 3
        }
        
        //создание потоков
        //потоки покупателей
	for (i = 0; i < NUM_OF_POKTHREADS; i++) {
		status1 = pthread_create(&pok_threads[i], NULL, buy, &p_param[i]);
		
		if (status1 != 0) {
			printf("main error1: can't create thread, status = %d\n", status1);
			handle_error("pthread_create1");
		}			
	}
	//потоки грузчиков
	for (i = 0; i < NUM_OF_GRUTHREADS; i++) {
		status2 = pthread_create(&gru_threads[i], NULL, give, &g_param[i]);
		if (status2 != 0) {
			printf("main error2: can't create thread, status = %d\n", status2);
			handle_error("pthread_create2");
		}
	}
	
	//ожидание завершения потоков
	for (i = 0; i < NUM_OF_POKTHREADS; i++) {
		status1 = pthread_join(pok_threads[i], NULL); 
		if (status1 != 0){
			printf("main error1: can't join thread, status = %d\n", status1);
			handle_error("pthread_join1");
		}
		printf("joined with address1 %d\n", status_addr1);
		status2 = pthread_join(gru_threads[i], NULL); //pthread_join(gru_threads[i],(void**)&status_addr1
		if (status2 != 0){
			printf("main error2: can't join thread, status = %d\n", status2);
			handle_error("pthread_join2");
		}
		printf("joined with address2 %d\n", status_addr2);
	}	
	
	//неприсоединенный поток
	//pthread_exit
	//
	
	//уничтожение мьютекса
	pthread_mutex_destroy(&mutex);
//	printf("counter = %d", counter);
	//getch();
	return 0;
}




