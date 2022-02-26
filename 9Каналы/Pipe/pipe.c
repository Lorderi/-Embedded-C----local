#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
	int fildes1[2], fildes2[2];
	pid_t cpid;
	size_t size1, size2;
	char msg1[] = "Hello";
	char msg2[] = "Hi";
	char string1[strlen(msg1)];
	char string2[strlen(msg2)]; 

	/* Попытаемся создать pipe 1*/
	if(pipe(fildes1) < 0){
		/* Если создать pipe 1 не удалось, печатаем об этом сообщение
		и прекращаем работу */
		printf("Can\'t create pipe\n");
		perror("pipe");
		exit(-1); //EXIT_FAILURE
	} 
	/* Попытаемся создать pipe 2*/
	if(pipe(fildes2) < 0){
		/* Если создать pipe 2 не удалось, печатаем об этом сообщение
		и прекращаем работу */
		printf("Can\'t create pipe\n");
		perror("pipe");
		exit(-1); //EXIT_FAILURE
	} 
	/* Порождаем новый процесс */ 
	cpid = fork();
	if(cpid == -1){ 
		/* Если создать процесс не удалось, сообщаем об этом и 
		завершаем работу */
		printf("Can\'t fork child\n");
		perror("fork");
		exit(-1);
	} else if (cpid > 0) {
		/* Мы находимся в родительском процессе, который будет 
		передавать информацию процессу-ребенку. В этом процессе
		выходной поток данных нам не понадобится, поэтому 
		закрываем его.*/
		close(fildes1[0]);
		close(fildes2[1]);
	
		/* Пробуем записать в pipe N байт, т.е. всю строку 
		"Hello" вместе с признаком конца строки */
		size1 = write(fildes1[1], msg1, strlen(msg1));
		if(size1 != strlen(msg1)){
			/* Если записалось меньшее количество байт, сообщаем
			об ошибке и завершаем работу */
			printf("Parent: Can\'t write all string\n"); 
			exit(-1); 
		}
		
		/*!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
		/* Пробуем прочитать из pip'а N байт в массив, т.е. всю
		записанную строку */
		size2 = read(fildes2[0], string2, strlen(msg2));
		if(size2 < 0){
			/* Если прочитать не смогли, сообщаем об ошибке и
			завершаем работу */
			printf("Parent: Can\'t read string\n"); 
			exit(-1); 
		}
		/* Печатаем прочитанную строку */
		printf("Parent: msg from daughter: %s\n", string2);
		/* Закрываем входной поток и завершаем работу */
		/*!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
		
		/* Закрываем входной поток данных, на этом 
		родитель прекращает работу */
		close(fildes1[1]);
		close(fildes1[0]);
		wait(NULL);/* Ожидание потомка */
		printf("Parent exit\n");
		exit(EXIT_SUCCESS);
	} else {
		/* Мы находимся в порожденном процессе, который будет 
		получать информацию от процесса-родителя. Он унаследовал
		от родителя таблицу открытых файлов и, зная файловые 
		дескрипторы, соответствующие pip, иможет его использовать.
		В этом процессе входной поток данных нам не 
		ипонадобится, поэтому закрываем его.*/
		close(fildes1[1]);
		close(fildes2[0]);
		/* Пробуем прочитать из pip'а N байт в массив, т.е. всю
		записанную строку */
		size1 = read(fildes1[0], string1, strlen(msg1));
		if(size1 < 0){
			/* Если прочитать не смогли, сообщаем об ошибке и
			завершаем работу */
			printf("daughter: Can\'t read string\n"); 
			exit(-1);
		}
		/* Печатаем прочитанную строку */
		printf("daughter: msg from parent: %s\n", string1);
		/* Закрываем входной поток и завершаем работу */
		
		/*!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
		/* Пробуем записать в pipe N байт, т.е. всю строку 
		"Hi" вместе с признаком конца строки */
		
		size2 = write(fildes2[1], msg2, strlen(msg2));
		if(size2 != strlen(msg2)){
			/* Если записалось меньшее количество байт, сообщаем
			об ошибке и завершаем работу */
			printf("daughter: Can\'t write all string\n"); 
			exit(-1); 
		} 
		/*!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
		
		close(fildes1[0]);
		close(fildes2[1]);
		printf("daughter exit\n");
		_exit(EXIT_SUCCESS);
	}    
	return 0; 
}
