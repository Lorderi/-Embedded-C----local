
//Более гибкая настройка по сравнению с сигналами: лишен недостатков:
/*
функция не блокирует получение других сигналов пока выполняется текущий обработчик, он будет прерван и начнет выполняться новый обработчик
после первого получения сигнала (для которого мы установили свой обработчик), его обработчик будет сброшен на SIG_DFL
*/
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


//обработчик - переопределение, прерывание значит что основная функция в этот момент ниче не делает
void hdl(int sig, siginfo_t *make, void *arg){
    //диспозиция(функция, код выполняющийся вов время приход сигнала, обработчик прерывания)
    printf("\nReceived signal: %d\n", sig);
    printf("From process %u\n", make->si_pid);
    printf("Done from function\n\n");
}

int main(void)
{
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    printf("I am process %i\n", getpid());

    sigact.sa_sigaction = &hdl; //либо более простой .sa_handler без доп. полей
    sigact.sa_flags = SA_SIGINFO; // флаг SIGINFO, вся эта информация будет доступна сразу, т.к. выводим информацию о процессе, который послал сигнал, то лучше этот флаг выставить
    sigaction(SIGUSR1, &sigact, 0); //обработчик для сигналов SIGUSR1
    
    while(1){
        printf(".");
        sleep(1);
    }


    printf("Done from main\n");
    return 0;
}
