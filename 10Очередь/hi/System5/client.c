/*
 * client.c: Client program
 *           to demonstrate interprocess commnuication
 *           with System V message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
#define PROJECT_ID 'M'

/*Структура сообщения*/
struct message_text {
    int qid; /*id очередь*/
    char buf [200]; /*буффер*/
};
/*Структура сообщения вместе с приоритетом*/
struct message {
    long message_type; /*приоритет*/
    struct message_text message_text; /*Структура сообщения*/
};

int main (int argc, char **argv)
{
    /*CREATE*/
    /*-----------------------------------------------------------------------*/
    key_t server_queue_key; /*Уникальный ключ*/
    int server_qid, myqid; /*id очередь сервера и клиента(своя)*/
    struct message my_message, return_message; /*Структура своего сообщения вместе с приоритетом, возвращенное сообщение*/

    // создание очереди КЛИЕНТА, идентификатор очереди, флаги(что делать, создать или присоединиться), права доступа к файлу: create my client queue for receiving messages from server
    //0660 чтение для группы и владельцы
    /*IPC_PRIVATE всегда приводит к попытке создания новой очереди сообщений с ключом, который не совпадает со значением ключа ни одной из уже существующей очередей и который не может быть получен с помощью функции ftok() ни при одной комбинации ее параметров.*/
    if ((myqid = msgget (IPC_PRIVATE, 0660)) == -1) {
        perror ("msgget: myqid");
        exit (1);
    }
	/*генерация уникального ключа СЕРВЕРА: имя файла и номер*/
    if ((server_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }
	//создание очереди СЕРВЕРА, идентификатор очереди, флаги(что делать, создать или присоединиться):
    if ((server_qid = msgget (server_queue_key, 0)) == -1) {
        perror ("msgget: server_qid");
        exit (1);
    }
	
    my_message.message_type = 1; /*Создание приоритета: 1*/
    my_message.message_text.qid = myqid; /*id очереди КЛИЕНТА*/
    
    /*CREATE_END*/
    /*-----------------------------------------------------------------------*/
    
    
    /*SEND/RECEIVE*/
    /*-----------------------------------------------------------------------*/

	/*буффер сообщения КЛИЕНТА*/
    
    /*fgets (my_message.message_text.buf, 198, stdout);
    printf("Hello server!");
    */
    sprintf(my_message.message_text.buf, "%s\n", "Hello server!");
   //my_message.message_text.buf= "Hello server!";
     
    // send message to server: id очереди, буфер, размер, флаги (0 - блокирование)
    if (msgsnd (server_qid, &my_message, sizeof (struct message_text), 0) == -1) {
        perror ("client: msgsnd");
        exit (1);
    }

        // read response from server: id очереди, буфер, размер, приоритет 0 - самое приоритетное -7 - ищем 7, если нет то 6 и тд, флаги (0 - блокирование)
    if (msgrcv (myqid, &return_message, sizeof (struct message_text), 0, 0) == -1) {
        perror ("client: msgrcv");
        exit (1);
    }

    // process return message from server
    printf ("Message received from server: %s\n\n", return_message.message_text.buf);  

    
    
    /*SEND/RECEIVE*/
    /*-----------------------------------------------------------------------*/
    
    // remove message queue: id очереди
    if (msgctl (myqid, IPC_RMID, NULL) == -1) {
            perror ("client: msgctl");
            exit (1);
    }

    printf ("Client: bye\n");

    exit (0);
}
