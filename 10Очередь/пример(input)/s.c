/*
 * server.c: Server program
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
#define QUEUE_PERMISSIONS 0660 //чтение для группы и владельцы

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
    key_t msg_queue_key; /*Уникальный ключ*/
    int qid; /*id очередь СЕРВЕРА*/
    struct message message; /*Структура сообщения с приоритетом*/
	/*генерация уникального ключа: имя файла и номер*/
    if ((msg_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }
	//создание очереди СЕРВЕРА, идентификатор очереди, флаги(что делать, создать или присоединиться):
	/*IPC_CREAT- если очередь для указанного ключа не существует, она должна быть создана.*/
    if ((qid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror ("msgget");
        exit (1);
    }

    printf ("Server: Hello, World!\n");

    while (1) {
        // read an incoming message: id очереди, буфер, размер, приоритет, флаги (0 - блокирование)
        if (msgrcv (qid, &message, sizeof (struct message_text), 0, 0) == -1) {
            perror ("msgrcv");
            exit (1);
        } /*получили сообщение от КЛИЕНТА*/

        printf ("Server: message received.\n");

        // process message
        int length = strlen (message.message_text.buf); /*длина сообщения*/
        char buf [20];
        sprintf (buf, " %d", length); /*запись в локальный буффер длины*/
        strcat (message.message_text.buf, buf); /*присоединение к принятому сообщению лок.буффер(длина)*/

        int client_qid = message.message_text.qid; /*id очереди КЛИЕНТА как id СЕРВЕРА*/
        message.message_text.qid = qid; /*очередь сообщения id сервера как в main*/

        // send reply message to client: id очереди, буфер, размер, флаги (0 - блокирование)
        if (msgsnd (client_qid, &message, sizeof (struct message_text), 0) == -1) {  
            perror ("msgget");
            exit (1);
        }

        printf ("Server: response sent to client.\n");
    }
}
