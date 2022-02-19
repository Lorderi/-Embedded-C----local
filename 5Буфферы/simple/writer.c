#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <malloc.h>

int main(){
//	void *buf;
	int fd, bytes;
	char buf[100] = {0};
//	buf = malloc(100);
        
        printf("Введите текст\n");
        gets(buf);
        
	if ((fd = open("testfile.txt",O_WRONLY | O_TRUNC | O_CREAT ))==-1) {
		perror("Ошибка при открытии файла.");
		exit (1);
	}
	if ((bytes = write(fd, buf, strlen(buf))) == -1){
		perror("Ошибка записи.\n");
		exit (1);
	}
	
	printf("Wrote %u bytes to file\n", bytes);
        close(fd);
        return 0;
}
