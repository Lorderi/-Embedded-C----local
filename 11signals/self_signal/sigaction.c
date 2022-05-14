
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


//обработчик - переопределение, прерывание значит что основная функция в этот момент ниче не делает
void hdl(int sig, siginfo_t *make, void *arg){
    //диспозиция(функция, код выполняющийся вов время приход сигнала, обработчик прерывания)
    printf("Received signal: %d\n", sig);
    printf("From process %u\n", make->si_pid);
    printf("Done from function\n");
}

int main(void)
{
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    printf("I am process %i\n", getpid());

    sigact.sa_sigaction = &hdl; //либо более простой .sa_handler без доп. полей
    //блокировка в маске работает так: получаем сигнал, вывывается код маски и блокируется что в маскке и выполняется диспозиция, потом восстанавливается и далее
    /*
    sigset_t set; 
    sigemptyset(&set); //очищение структуры                                                            
    sigaddset(&set, SIGUSR1); //необходимо блокировать эти же сигналы пока выполняется обработчик
    sigact.sa_mask = set; // маска сигналов который будут блокированы пока выполняется наш обработчик. + по дефолту блокируется и сам полученный сигнал
    */
    sigact.sa_flags = SA_SIGINFO; // флаг SIGINFO, вся эта информация будет доступна сразу, т.к. выводим информацию о процессе, который послал сигнал, то лучше этот флаг выставить
    sigaction(SIGUSR1, &sigact, 0); //обработчик для сигналов SIGUSR1
    kill(getpid(), SIGUSR1); //посылаем сигнал себе
    printf("Done from main\n");

    return 0;
}
