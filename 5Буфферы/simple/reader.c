#include<stdio.h>
#include <stdlib.h>
#include<fcntl.h>

int main()
{
   //void *buf;
   int fd,bytes;
   char buf[100];
   //buf = malloc(100);
   if((fd = open("testfile.txt",O_RDONLY))== -1)
   {
      perror("Ошибка при открытии файла.\n");
      exit(1);
   }
   if((bytes = read(fd,buf,sizeof(buf)))==-1)
   {
      perror("Ошибка чтения.\n");
      exit(1);
   }
   printf("Считано %d байт.\nСТрока:%s\n", bytes,buf);
   return 0;
}
