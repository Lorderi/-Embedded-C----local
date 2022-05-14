/*
 * client.c: Client program
 *           to demonstrate interprocess communication
 *           with POSIX message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
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

/*
typedef struct messege{
    char header[32];
    char mesg[256];
} msg_t;
*/
typedef struct send {
    char nickname_buf[32];
    int server_pid;
} snd_t;



//ФУНКЦИЯ ОТПРАВКЕ ОТ КЛИЕНТА СООБЩЕНИЯ
void * send_msg(void * args)
{   
    snd_t * lsnd = (snd_t *) args;
    //char *lnickname_buf = (char*)args;
    //msg_t msg;
    //memset(&msg, 0, sizeof(struct messege));
    char header[36] = {0};
    char allmsg[300] = {0};
    char msg[256] = {0};
    int lspid = lsnd->server_pid;
    //char *messg = allmsg + 32 + strlen(": "); //Отступаем на размер вводного ника 32 + символы : 
    
    strcpy(header,lsnd->nickname_buf); //вставляем ник в начало сообщения
    int length = strlen (header);
    if (header [length - 1] == '\n')
        header [length - 1] = '\0';

    strcat(header, ": ");

    mqd_t qd_server2_sndmsg;
    //ОЧЕРЕДЬ ДЛЯ ОТПРАВКИ: ОТПРАВКА СООБЩЕНИЯ В ЧАТ
    if ((qd_server2_sndmsg = mq_open (SERVER_QUEUE_NAME2, O_WRONLY)) == -1) {
        perror ("send_msg: mq_open (server)");
        exit (1);
    }
    
    //sleep(1);
    //printf("Введите сообщение:\n");
    while (fgets (msg, 256, stdin)) {
        if (strcmp(msg, "\n") == 0){
            printf("\nВы не написали сообщение\nВведите сообщение(exit - выйти):\n");
        }
        else if (strcmp(msg, "exit\n") == 0){
            printf("\nПроизводится выход из чата\n");
            break;
        }
        else{
            printf("\n");
            strcpy(allmsg, header);
            strcat(allmsg, msg);
            printf("Отправляемое сообщение: %s\n\n", allmsg);
            kill(lspid, SIGUSR1); //посылаем сигнал
            if (mq_send (qd_server2_sndmsg, allmsg, strlen(allmsg) + 1, 0) == -1) {
                perror ("send_msg: mq_send MSG");   
            }
            //sleep(1); //чтобы подождать пока буфер прийдет и вывести сообщение, чтобы не перекрывалось
            //printf("Введите сообщение:\n");
        }
    }



    if (mq_close (qd_server2_sndmsg) == -1) {
        perror ("send_msg: mq_close");
        exit (1);
    }

    if (mq_unlink (SERVER_QUEUE_NAME2) == -1) {
        perror ("send_msg: mq_unlink");
        exit (1);
    }
    printf ("Client(send_msg): bye\n");
    return 0;
}

//ФУНКЦИЯ ПО ПРИЕМУ СООБЩЕНИЙ И ЛИСТА ПОЛЬЗОВАТЕЛЕЙ
void * receive_msg_list(void * args)
{
    snd_t * lsnd = (snd_t *) args;
    char client_queue_name[64] = {0};
    mqd_t qd_client;   // дескрипторы очереди

    
    // создать клиентскую очередь для получения сообщений с сервера
    sprintf (client_queue_name, "/sp-example-client-%d", getpid());
    
    /*Значение mq_maxmsg в структуре, указатель которой передается как attr в mq_open , должно быть <= значению в интерфейсном файле /proc, / proc/sys/fs/mqueue/msg_max (значение по умолчанию в файле равно 10). Точно так же значение mq_msgsize должно быть меньше, чем в файле /proc/sys/fs/mqueue/msgsize_max , значение по умолчанию в файле составляет 8192 байта. Эти ограничения игнорируются для привилегированных процессов.*/

    struct mq_attr attr;

    attr.mq_flags = 0;  /* Flags: 0 or O_NONBLOCK */
    attr.mq_maxmsg = MAX_MESSAGES; /* Max. # of messages on queue */
    attr.mq_msgsize = MAX_MSG_SIZE; /* Max. message size (bytes) */
    attr.mq_curmsgs = 0; /* # of messages currently in queue */

    //ОЧЕРЕДЬ НА ПРИЕМ ДАННЫХ ОТ СЕРВЕРА: СПИСОК ПОЛЬЗОВАТЕЛЕЙ И ЧАТ
    if ((qd_client = mq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("receive_msg_list: mq_open (client)");
        exit (1);
    }
    char list_of_users[4096] = {0};
    char msg_buff[4096] = {0};
    kill(lsnd->server_pid, SIGUSR1); //посылаем сигнал
    while(1){

        if (mq_receive (qd_client, list_of_users, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("receive_msg_list: mq_receive USR");
            exit (1);
        }
        printf ("Client(receive_msg_list): Пользователи в чате:\n%s\n\n", list_of_users);
        if (mq_receive (qd_client, msg_buff, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("receive_msg_list: mq_receive MSGS");
            exit (1);
        }
        if (! msg_buff[0]){ //если указателя на первый нет то это \0
            printf ("Буффер сообщений пуст, ещё никто не написал\n\n");
        }
        else {
            printf ("Client(receive_msg_list): Сообщения:\n%s\n\n", msg_buff);
        }
        printf("Введите сообщение(exit - выйти):\n");
        
    }


    if (mq_close (qd_client) == -1) {
        perror ("receive_msg_list: mq_close");
        exit (1);
    }

    if (mq_unlink (client_queue_name) == -1) {
        perror ("receive_msg_list: mq_unlink");
        exit (1);
    }
    printf ("Client(receive_msg_list): bye\n");
    return 0;
}



int main ()
{

    pthread_t thread_rc, thread_snd; //pthread - покупатели gthread - грузчики// указатель на поток, id потока
	int i = 0, status1, status2;
    int stat_addr;
   

    char client_queue_name[64] = {0};
    mqd_t qd_server1_sndque, qd_server2_sndmsg, qd_client;   // дескрипторы очереди
    
    //структура для отправки в поток пида сервера и имени очереди
    snd_t snd;
    snd.nickname_buf[32];
    snd.server_pid;
    memset(&snd, 0, sizeof(struct send));

    char cserver_pid[64];

    // создать клиентскую очередь для получения сообщений с сервера
    sprintf (client_queue_name, "/sp-example-client-%d", getpid ());
    
    /*Значение mq_maxmsg в структуре, указатель которой передается как attr в mq_open , должно быть <= значению в интерфейсном файле /proc, / proc/sys/fs/mqueue/msg_max (значение по умолчанию в файле равно 10). Точно так же значение mq_msgsize должно быть меньше, чем в файле /proc/sys/fs/mqueue/msgsize_max , значение по умолчанию в файле составляет 8192 байта. Эти ограничения игнорируются для привилегированных процессов.*/

    struct mq_attr attr;

    attr.mq_flags = 0;  /* Flags: 0 or O_NONBLOCK */
    attr.mq_maxmsg = MAX_MESSAGES; /* Max. # of messages on queue */
    attr.mq_msgsize = MAX_MSG_SIZE; /* Max. message size (bytes) */
    attr.mq_curmsgs = 0; /* # of messages currently in queue */


    /* 1- имя очереди;2 - флаг, O_RDONLY для получения сообщений, O_WRONLY для отправки сообщений и O_RDWR для операций отправки и получения в очереди.O_NONBLOCK , чтобы использовать очередь в неблокирующем режиме. По умолчанию mq_send блокируется, если очередь заполнена, а mq_receive блокируется, если в очереди нет сообщений. Но если в oflag указан O_NONBLOCK, в этих случаях вызов будет немедленно возвращен, если для errno установлено значение EAGAIN .

    Если вы укажете O_CREAT как часть oflag , очередь будет создана, если она еще не существует. Если вы укажете O_EXCL вместе с O_CREAT и очередь существует, mq_open завершится ошибкой, а для errno будет установлено значение EEXIST . Если в oflag указан O_CREAT , необходимо использовать вторую форму mq_open с двумя дополнительными параметрами.

    3 - разрешения для очереди, 4 -указатель на структуру mq_attr дает атрибуты для очереди сообщений. Если этот указатель имеет значение NULL, создается очередь с атрибутами по умолчанию.*/


    //ОЧЕРЕДЬ НА ПРИЕМ ДАННЫХ ОТ СЕРВЕРА: СПИСОК ПОЛЬЗОВАТЕЛЕЙ И ЧАТ
    if ((qd_client = mq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Client: mq_open (client)");
        exit (1);
    }
    //ОЧЕРЕДЬ НА ОТПРАВКУ ДАННЫХ ДЛЯ РЕГИСТРАЦИИ В ЧАТЕ
    if ((qd_server1_sndque = mq_open (SERVER_QUEUE_NAME1, O_WRONLY)) == -1) {
        perror ("Client: mq_open (server)");
        exit (1);
    }
    /*
    //ОЧЕРЕДЬ НА ОТПРАВКУ СООБЩЕНИЙ В ЧАТ
    if ((qd_server2_sndmsg = mq_open (SERVER_QUEUE_NAME2, O_WRONLY)) == -1) {
        perror ("Client: mq_open (server)");
        exit (1);
    }
    */
    //char in_buffer[MSG_BUFFER_SIZE];


    


    
    
    /*1 - дескриптор mqdes; 2 - буфер сообщений; 3 - размер сообщения, который должен быть меньше или равен размеру сообщения для очереди; 4- приоритет сообщения, представляющий собой неотрицательное число, указывающее приоритет сообщения. Сообщения помещаются в очередь в порядке убывания приоритета сообщений, при этом более старые сообщения с приоритетом располагаются перед более новыми сообщениями. Если очередь заполнена, mq_send блокируется до тех пор, пока в очереди не останется места, если только для очереди сообщений не установлен флаг O_NONBLOCK , в этом случае mq_send немедленно возвращается с errno установленным в EAGAIN .*/
	
    // отправить сообщение на сервер
    if (mq_send (qd_server1_sndque, client_queue_name, strlen (client_queue_name) + 1, 0) == -1) {
        perror ("Client: Not able to send message to server");
    }
    
    
    if (mq_receive (qd_client, cserver_pid, MSG_BUFFER_SIZE, NULL) == -1) {
        perror ("Client: mq_receive");
        exit (1);
    }
    snd.server_pid = atoi(cserver_pid);
    printf("Client: pid сервера: %d\n\n", snd.server_pid);

    printf("Введите никнейм:\n");
    fgets(snd.nickname_buf, 32, stdin);
    printf("\n");

    // отправить сообщение на сервер
    if (mq_send (qd_server1_sndque, snd.nickname_buf, strlen (snd.nickname_buf) + 1, 0) == -1) {
        perror ("Client: Not able to send message to server");
    }

        /*
        if (mq_receive (qd_server1_sndque, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }*/
        // отображать токен, полученный от сервера
        //printf ("Client: Token received from server: %s\n\n", in_buffer);
    
    for (i = 0; i < NUM_OF_THREAD; i++) {
		status1 = pthread_create(&thread_rc, NULL, send_msg, &snd);
		
		if (status1 != 0) {
			printf("main error1: can't create thread, status = %d\n", status1);
			handle_error("pthread_create1");
		}			
	}
    for (i = 0; i < NUM_OF_THREAD; i++) {
		status1 = pthread_create(&thread_snd, NULL, receive_msg_list, &snd);
		
		if (status1 != 0) {
			printf("main error1: can't create thread, status = %d\n", status1);
			handle_error("pthread_create1");
		}			
	}

    
    printf("Client: Ждем snd_join\n");
    /*
    for (i = 0; i < NUM_OF_THREAD; i++) {
		status1 = pthread_join(thread_snd, (void**)&stat_addr);
        printf("Client: pthread_join\n");
		if (status1 != 0){
			printf("main error1: can't join thread, status = %d\n", status1);
			handle_error("pthread_join");
		}
        if (stat_addr == -1){
            printf("Client: thread_snd закончен\n");
        }                                                                                             
        else{
            printf("Client: thread_snd не закончен\n");
        }                                                                          
    }
    */

    status1 = pthread_join(thread_snd, (void**)&stat_addr);
    printf("Client: pthread_join\n");
	if (status1 != 0){
		printf("main error1: can't join thread, status = %d\n", status1);
		handle_error("pthread_join");
	}
    if (stat_addr == -1){
        printf("Client: thread_snd закончен\n");
    }                                                                                             
    else{
        printf("Client: thread_snd не закончен\n");
    }                            
    
    printf("Client: pthread_join end\n");

    if (pthread_cancel(thread_rc) == -1) {                                            
        perror("Client: pthread_cancel");                                             
        exit(1);                                                                     
    }

    for (i = 0; i < NUM_OF_THREAD; i++) {
		status1 = pthread_join(thread_rc, 0); //отвязываем чтобы дошло до джоина, и если джоин вернется всё завершилось
		if (status1 != 0){
			printf("main error1: can't detach thread, status = %d\n", status1);
			handle_error("pthread_detach");
		}
    }

    printf("Client: Потоки завершились\n");


    if (mq_close (qd_client) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }

    if (mq_unlink (client_queue_name) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
    printf ("Client: bye\n");
    return 0;
}

