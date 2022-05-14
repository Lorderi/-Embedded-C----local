/*
signal
функция не блокирует получение других сигналов пока выполняется текущий обработчик, он будет прерван и начнет выполняться новый обработчик
после первого получения сигнала (для которого мы установили свой обработчик), его обработчик будет сброшен на SIG_DFL
*/
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
//обработчик - переопределение, прерывание значит что основная функция в этот момент ниче не делает
void listener(int sig) {
    fprintf(stdout, "\nlistener\n\n");
}
 
void main() {
    int status;
    //устанавливаем обработчик для сигнала SIGINT
    status = signal(SIGINT, listener);
    if (SIG_ERR == status) {
        fprintf(stdout, "error signal");
        exit(1);
    }
    printf("begin\n");
    //посылаем сигнал
    
    while(1){
        printf(".");
        sleep(1);
    }

    fprintf(stdout, "end\n");
}