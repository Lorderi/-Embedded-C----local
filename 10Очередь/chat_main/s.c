/*
 * server.c: Server program
 *           to demonstrate interprocess commnuication
 *           with POSIX message queues
 */
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define NUM_OF_THREAD 1

#define SERVER_QUEUE_NAME1   "/sp-example-server-start-rc"
#define SERVER_QUEUE_NAME2   "/sp-example-server-msg-rc"

#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 4096
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

char list_of_users[2048] = {0};
char msg_buff[2048] = {0};

//обработчик - переопределение, прерывание значит что основная функция в этот момент ниче не делает
void hdl(int sig, siginfo_t *make, void *arg){
    //диспозиция(функция, код выполняющийся вов время приход сигнала, обработчик прерывания)
    //printf("Received signal: %d\n", sig);
    //printf("From process %u\n", make->si_pid);
    printf("Done from function\n");
}


//ФУНКЦИЯ ПО ПРИЕМУ ОТ КЛИЕНТА СООБЩЕНИЙ
void * receive_msg(void *args)
{
    
    mqd_t qd_server2_recmsg;

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    //ОЧЕРЕДЬ ДЛЯ ПРИЁМА: ПРИЁМ НА СООБЩЕНИЯ В ЧАТ
    if ((qd_server2_recmsg = mq_open (SERVER_QUEUE_NAME2, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("receive_msg: mq_open (server)");
        exit (1);
    }
    char loc_msg_buff[MSG_BUFFER_SIZE];
    //char out_buffer [MSG_BUFFER_SIZE];
    while(1){

        if (mq_receive (qd_server2_recmsg, loc_msg_buff, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("receive_msg: mq_receive MSG");
            exit (1);
        }
        printf ("Server(receive_msg): Принятое сообщение: %s\n\n", loc_msg_buff);
        strcat(msg_buff, loc_msg_buff);
        printf ("Server(receive_msg): Сообщение добавлено в буфер\n\n");
    }


    if (mq_close (qd_server2_recmsg) == -1) {
        perror ("receive_msg: mq_close");
        exit (1);
    }

    if (mq_unlink (SERVER_QUEUE_NAME2) == -1) {
        perror ("receive_msg: mq_unlink");
        exit (1);
    }
    printf ("receive_msg: bye\n");
}

//ФУНКЦИЯ ПО ОТПРАВКЕ СООБЩЕНИЙ И ЛИСТА ПОЛЬЗОВАТЕЛЕЙ
void * send_msg_list(void *args)
{
    mqd_t qd_client;   // queue descriptors
    char *lque_buffer = (char*)args;
    char locmsg_buff[MSG_BUFFER_SIZE] = {1}; //заполнено единицами, чтобы потом провести проверку сравнения с заполненным нулями

    //ПОТОКИ ДОЛЖНЫ НАСЛЕДОВАТЬ АДРЕСНОЕ ПРОСТРАНСТВО РОДИТЕЛЯ
    //КАК ЭТО СДЕЛАТЬ Я НЕ ЗНАЮ, ПОЭТОМУ ИНИЦИАЛИЗИРУЕМ ЗАНОВО
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    int sig;
    sigact.sa_sigaction = &hdl; //либо более простой .sa_handler без доп. полей(инфа и инфа)
    //блокировка в маске работает так: получаем сигнал, вывывается код маски и блокируется что в маскке и выполняется диспозиция, потом восстанавливается и далее
    
    sigset_t set; 
    sigemptyset(&set); //очищение структуры                                                            
    sigaddset(&set, SIGUSR1); //необходимо блокировать эти же сигналы пока выполняется обработчик
    //либо так: надо или нет не знаю
    //sigact.sa_mask = set; // маска сигналов который будут блокированы пока выполняется наш обработчик. + по дефолту блокируется и сам полученный сигнал
    sigact.sa_flags = SA_SIGINFO; // флаг SIGINFO, вся эта информация будет доступна сразу, т.к. выводим информацию о процессе, который послал сигнал, то лучше этот флаг выставить

    //sigaction(SIGUSR1, &sigact, 0); //обработчик для сигналов SIGUSR1
    int note = 0;




    while(1){

        //СОЗДАНИЕ ОЧЕРЕДИ КЛИЕНТА ДЛЯ ЗАПИСИ ТУДА
        if ((qd_client = mq_open (lque_buffer, O_WRONLY)) == 1) {
            perror ("send_msg_list: mq_open");
            continue;
        }

        
        //ПОКА ПОЛЬЗОВАТЕЛЬ НЕ ВЫЙДЕТ
        while(1){
            if (note == 20){
                sigwait(&set, &sig); //засыпает пока сигнал не придет и не нагружает систему
                note = 0;
            }
            
            //проверка, если сообщение изменилось
            //После включения SIGWAIT условие на то, что если сообщение повторяется - не реагировать
            //перестало работать. возможно потому, что сигналы влияют так
            if (strcmp(locmsg_buff, msg_buff) != 0){
                //printf("Server(send_msg_list):\n-------------------------\n%s\n\n", lque_buffer);
                //ОТПРАВКА СПИСКА КЛИЕНТОВ
                if (mq_send (qd_client, list_of_users, strlen (list_of_users) + 1, 0) == -1) {
                    perror ("send_msg_list: mq_send LIST");   
                }
                //ОТПРАВКА СПИСКА СООБЩЕНИЙ
                if (mq_send (qd_client, msg_buff, strlen (msg_buff) + 1, 0) == -1) {
                    perror ("send_msg_list: mq_send MSG");
                }
                printf ("Server(send_msg_list): Список пользователей и сообщения отправлены\n\n");
                //sleep(3); //Чтобы комп не взорвался от постоянного цикла проверки
            }
            else{note++;}
            memcpy(locmsg_buff, msg_buff, strlen(msg_buff) + 1);
            
        }
        continue;
    }

    if (mq_close (qd_client) == -1) {
        perror ("send_msg_list: mq_close");
        exit (1);
    }

    if (mq_unlink (lque_buffer) == -1) {
        perror ("send_msg_list: mq_unlink");
        exit (1);
    }
    printf ("send_msg_list: bye\n");


    
}










int main (int argc, char **argv)
{
    pthread_t thread_rc, thread_snd; //pthread - покупатели gthread - грузчики// указатель на поток, id потока
	int i; //счетчик
	int status1, status2;


    mqd_t qd_server1_rcreg, qd_server2_recmsg, qd_client;   // queue descriptors

    printf ("Server: включен\n");

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    //ОЧЕРЕДЬ ДЛЯ ПРИЁМА: ПРИЁМ НА РЕГИСТРАЦИЮ в ЧАТЕ
    if ((qd_server1_rcreg = mq_open (SERVER_QUEUE_NAME1, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    /*
    //ОЧЕРЕДЬ ДЛЯ ПРИЁМА: ПРИЁМ НА СООБЩЕНИЯ В ЧАТ
    if ((qd_server2_recmsg = mq_open (SERVER_QUEUE_NAME2, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    */
    char que_buffer[MSG_BUFFER_SIZE] = {0};
    char rc_buffer[MSG_BUFFER_SIZE] = {0};
    
    char pid_buffer[64] = {0};
    
    
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    int sig;
    sigact.sa_sigaction = &hdl; //либо более простой .sa_handler без доп. полей(инфа и инфа)
    //блокировка в маске работает так: получаем сигнал, вывывается код маски и блокируется что в маскке и выполняется диспозиция, потом восстанавливается и далее
    
    sigset_t set; 
    sigemptyset(&set); //очищение структуры                                                            
    sigaddset(&set, SIGUSR1); //необходимо блокировать эти же сигналы пока выполняется обработчик
    pthread_sigmask(SIG_BLOCK, &set, 0); //sigprocmask
    //либо так: надо или нет не знаю
    //sigact.sa_mask = set; // маска сигналов который будут блокированы пока выполняется наш обработчик. + по дефолту блокируется и сам полученный сигнал
    sigact.sa_flags = SA_SIGINFO; // флаг SIGINFO, вся эта информация будет доступна сразу, т.к. выводим информацию о процессе, который послал сигнал, то лучше этот флаг выставить

    sprintf(pid_buffer, "%d", getpid());

    while (1) {
        


        //ПРИЕМ ID ОЧЕРЕДИ КЛИЕНТА ДЛЯ СОЗДАНИЯ ОЧЕРЕДИ
        if (mq_receive (qd_server1_rcreg, que_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Server: mq_receive");
            exit (1);
        }

        printf ("Server: Имя очереди принято: %s\n\n", que_buffer);
        
        //СОЗДАНИЕ ОЧЕРЕДИ КЛИЕНТА ДЛЯ ЗАПИСИ ТУДА СВОЕГО pid
        if ((qd_client = mq_open (que_buffer, O_WRONLY)) == 1) {
            perror ("send_msg_list: mq_open");
        }
        
        if (mq_send (qd_client, pid_buffer, strlen (pid_buffer) + 1, 0) == -1) {
            perror ("Server: send pid");
        }
        printf ("Server: pid сервера отправлен: %s\n\n", pid_buffer);
        //создаем здесь, чтобы клиент к этому моменту мог уже подключиться
        for (i = 0; i < NUM_OF_THREAD; i++) {
            status1 = pthread_create(&thread_rc, NULL, receive_msg, NULL);
            
            if (status1 != 0) {
                printf("main error1: can't create thread, status = %d\n", status1);
                handle_error("pthread_create1");
            }			
	    }
        /*
        //СОЗДАНИЕ ОЧЕРЕДИ КЛИЕНТА ДЛЯ ЗАПИСИ ТУДА
        if ((qd_client = mq_open (que_buffer, O_WRONLY)) == 1) {
            perror ("Server: Not able to open client queue");
            continue;
        }
        */
        //ПРИЕМ С ОЧЕРЕДИ НИКА ДЛЯ РЕГИСТРАЦИИ НА СЕРВЕРЕ
        if (mq_receive (qd_server1_rcreg, rc_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Server: mq_receive");
            exit (1);
        }

        printf ("Server: Ник принят: %s\n\n", rc_buffer);

        //ДОБАВЛЕНИЕ НИКА В СПИСОК ПОЛЬЗОВАТЕЛЕЙ
        strcat(list_of_users, rc_buffer);

        printf ("Server: Ник добавлен в пул ников\n\n");

        //???
        /*
        if (mq_send (qd_client, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
            perror ("Server: Not able to send message to client");
            continue;
        }
        */
       



        

        for (i = 0; i < NUM_OF_THREAD; i++) {
            status1 = pthread_create(&thread_snd, NULL, send_msg_list, &que_buffer);
            
            if (status1 != 0) {
                printf("main error1: can't create thread, status = %d\n", status1);
                handle_error("pthread_create1");
		    }			
	    }

        for (i = 0; i < NUM_OF_THREAD; i++) {
            status1 = pthread_detach(thread_rc); 
            if (status1 != 0){
                printf("main error1: can't join thread, status = %d\n", status1);
                handle_error("pthread_join1");
            }
        }
        for (i = 0; i < NUM_OF_THREAD; i++) {
            status1 = pthread_detach(thread_snd); 
            if (status1 != 0){
                printf("main error1: can't join thread, status = %d\n", status1);
                handle_error("pthread_join1");
            }
        }

        if (mq_close (qd_client) == -1) {
            perror ("Server: mq_close");
            exit (1);
        }
        /*
        if (mq_unlink (que_buffer) == -1) {
            perror ("Server: mq_unlink");
            exit (1);
        }*/
    }






    

    if (mq_close (qd_server1_rcreg) == -1) {
        perror ("Server: mq_close");
        exit (1);
    }

    if (mq_unlink (SERVER_QUEUE_NAME1) == -1) {
        perror ("Server: mq_unlink");
        exit (1);
    }
    printf ("Server: bye\n");

    return 0;

}

