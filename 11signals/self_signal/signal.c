#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
///обработчик - переопределение, прерывание значит что основная функция в этот момент ниче не делает
void listener(int sig) {
    fprintf(stdout, "listener\n");
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
    
    status = kill(getpid(), SIGINT); //либо raise(SIGINT)
    if (0 != status) {
        fprintf(stdout, "error raise");
        exit(1);
    }
    fprintf(stdout, "end\n");
}