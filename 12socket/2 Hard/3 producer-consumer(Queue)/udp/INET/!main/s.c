#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <mqueue.h>
//Сокеты
#define NUM_OF_THREADS 10
#define NUM_PORT 9002
//Очереди
#define SERVER_QUEUE_NAME   "/sp-example-server"
#define QUEUE_PERMISSIONS 0777 //чтение для группы и владельцы
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)
//ЗАНИМАЕТ МНОГО РЕСУРСОВ ПРОЦЕССОРА, НУЖНО СДЕЛАТЬ КАК ТО ОЖИДАНИЕ ПОТОКОВ
           
           
//Почему то всегда с 9003 порта сабсервер, несмотря на то что создан пул портов. Возможно потому что обрабатываются запуски клиентов последовательно, а не параллельно
//pthread_cond_wait pthread_cond_signal в месте где постоянно проделывается if чтобы ресурсы сильно не загружались

// можно сделать пул структур в некоторое число, чтобы был меньше шанс зачистить старую новой и использования не тех данных потоком. закидывать в структуры по кругу.
//int tag_table[NUM_OF_THREADS] = {2};
int port_table[256] = {0}; //таблица портов 
int num_port = NUM_PORT + 1;
pthread_mutex_t mutex1;//, mutex2;


typedef struct serv_tag {
	int thread_id;
} servarg_t;


void* service(void *args) {
	/*инициализируем структуру локальной функции*/
	servarg_t *arg = (servarg_t*) args; //переданные аргументы в функцию
	struct sockaddr_in subserver_address, client_address;
	
	char s_message[50] = "Hello client from subserver!\n"; //отправляемое сообщение(от сервера)
	char client_response[256]; //буфер принимаемого сообщения(отклик клиента)
	/*Это буффер портов, сделано так, чтобы порты не простаивали*/
	
	
	
	int i = 0;
	int port;
	
	while(i < 256) {
		if(port_table[i] == 0) {
			pthread_mutex_lock(&mutex1);
			port_table[i] = num_port + i;
			port = port_table[i];
			pthread_mutex_unlock(&mutex1);
			break;
		}
		i++;
	}
	printf("Num_of_port:%d\n", port);
	//указать адрес для сокета
	
	//СОКЕТЫ
	//-------------------------------------------------------
	memset(&subserver_address, 0, sizeof(struct sockaddr_in)); /*Зануление структуры, очистка*/
	memset(&client_address, 0, sizeof(struct sockaddr_in));
	client_address.sin_family = AF_INET;

	subserver_address.sin_family = AF_INET;
	subserver_address.sin_port = htons(port);
	subserver_address.sin_addr.s_addr = htonl(INADDR_ANY);
	//inet_aton("192.168.8.150", &server_address.sin_addr);		
	int fd;
	fd = socket(AF_INET, SOCK_DGRAM, 0); //создание сервер сокета
	if (fd == -1)
		handle_error("subserver socket");
	//привязать сокет к указанному нами IP и порту
	if (bind(fd, (struct sockaddr *) &subserver_address, sizeof(struct sockaddr_in)) == -1)
		handle_error("bind");

	// второй агрумент это бэклог - сколько соединений может одновременно ожидать этот сокет
	
	socklen_t client_addr_size = sizeof(struct sockaddr_in); /*размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.*/
	//ВОЗМОЖНО ВМЕСТО sockaddr_in НАДО client_address	
	//-------------------------------------------------------
	
	
	//ОЧЕРЕДИ
	//-------------------------------------------------------
	mqd_t qd_server, qd_client;   // дескрипторы очереди
	if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY)) == -1) {
        perror ("Client: mq_open (server)");
        exit (1);
    }
	
	printf("pthread id: %d\n", arg->thread_id);
	for(;;){
			if (mq_receive (qd_server, client_response, MSG_BUFFER_SIZE, NULL) == -1) {
				perror ("Client: mq_receive");
				exit (1);
			}

			else{

				// отображать токен, полученный от сервера
				printf ("Client: msg received from server: %s\n\n", client_response);
				
				char port[32], ip[32];
				
				char *pch = strtok (client_response,"\n"); // во втором параметре указаны разделитель (пробел, запятая, точка, тире)
				strcpy(ip, pch);
				printf("ip:%s\n", ip);
				pch = strtok (NULL, "\n");
				strcpy(port, pch);
				client_address.sin_port = atoi(port);
				inet_aton(ip, &client_address.sin_addr);
				printf("ip:%s;port:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));


				if (sendto(fd, s_message, sizeof(s_message), 0, (struct sockaddr *) &client_address, client_addr_size) == -1) {
					perror ("subserver: sendto");
					exit (1);
				}

				//sendto(fd, s_message, sizeof(s_message), 0, (struct sockaddr *) &client_address, client_addr_size); //последний аргумент без указателя - ругается компилятор?????????????
			
			
				int bytes_read = recvfrom(fd, &client_response, sizeof(client_response), 0, (struct sockaddr *) &client_address, &client_addr_size);
				if(bytes_read <= 0) {
					printf ("SUBSERVER: Error: Nothing sent\n");
					handle_error("subserver recvfrom");
				}
				else printf("SUBSERVER: The client sent the data: %s\n", client_response);
				
				
					
				printf("SUBSERVER IP address(ntoa): %s\n", inet_ntoa(subserver_address.sin_addr));
				printf("SUBSERVER port     (ntons): %d\n", ntohs(subserver_address.sin_port));
				
				printf("CLIENT IP address(ntoa): %s\n", inet_ntoa(client_address.sin_addr));
				printf("CLIENT port     (ntohs): %d\n", ntohs(client_address.sin_port));
				pthread_mutex_lock(&mutex1);
				port_table[i] = 0;
				pthread_mutex_unlock(&mutex1);
			}
			
		}
		close(fd);
		if (mq_close (qd_client) == -1) {
       		perror ("Client: mq_close");
        	exit (1);
    	}
	}





int main(){
	//СОКЕТЫ
	//-------------------------------------------------------
	int status1, status2; //статус
	/*Т.к. мы не знаем сколько будет потоков, т.к. в нашей схеме оно образуется по мере пополнения, то выставляем произвольную цифру, на которой в конце цикла, при дохождении до 255 будет обнуление. Сделано это потому что динамику пока что не знаю, надо по факту сделать динамический.
	В данном решении задачи могут возникнуть проблемы, т.к. если дойдет до 255, а поток с 0 будет всё так же занят то пойдет коллизия, и id потоков не будут присваиваться*/
	servarg_t options[NUM_OF_THREADS];
	pthread_t serv_threads[NUM_OF_THREADS]; //id потоков
	int i = 0; //номера в id потоков serv_threads. проходят до 255 и обнуляются. Для id потоков
	
	char client_sockresponse[256]; //буфер принимаемого сообщения(отклик клиента)
	char client_queresponse[256];

	socklen_t client_addr_size = sizeof(struct sockaddr_in); /*размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.*/	

	//указать адрес для сокета
	struct sockaddr_in server_address, client_address;
	
	memset(&server_address, 0, sizeof(struct sockaddr_in)); /*Зануление структуры, очистка*/
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(NUM_PORT);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	//inet_aton("192.168.8.150", &server_address.sin_addr);
	
	int fd;
	int bytes_read;
	fd = socket(AF_INET, SOCK_DGRAM, 0); //создание сервер сокета
	if (fd == -1)
		handle_error("socket");
		
	//привязать сокет к указанному нами IP и порту
	if (bind(fd, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in)) == -1)
		handle_error("bind");
	//-------------------------------------------------------

	//ОЧЕРЕДИ
	//-------------------------------------------------------
	mqd_t qd_server;   // дескрипторы очереди
	//СТРУКТУРА ОЧЕРЕДИ ДЛЯ CREATE
    struct mq_attr attr;

    attr.mq_flags = 0;  /* Flags: 0 or O_NONBLOCK */
    attr.mq_maxmsg = MAX_MESSAGES; /* Max. # of messages on queue */
    attr.mq_msgsize = MAX_MSG_SIZE; /* Max. message size (bytes) */
    attr.mq_curmsgs = 0; /* # of messages currently in queue */

	//СОЗДАНИЕ ОЧЕРЕДИ
    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_WRONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
	//-------------------------------------------------------

	
	//ВОЗМОЖНО ВМЕСТО sockaddr_in НАДО client_address
	
	if ((pthread_mutex_init(&mutex1, NULL)) != 0){
			handle_error("mutex1_init");
	}

	for (i = 0; i < NUM_OF_THREADS; i++) {
		options[i].thread_id = i + 1;
	}
	
	for (i = 0; i < NUM_OF_THREADS; i++) {
		status1 = pthread_create(&serv_threads[i], NULL, service, &options[i]);
		
		if (status1 != 0) {
			printf("main error1: can't create thread, status = %d\n", status1);
			handle_error("pthread_create1");
		}			
	}
	

	
		
	for(;;) {
		bytes_read = recvfrom(fd, &client_sockresponse, sizeof(client_sockresponse), 0, (struct sockaddr *) &client_address, &client_addr_size);
		if(bytes_read <= 0) {
			printf ("SERVER: Error: Nothing sent, code :%d\n", bytes_read);
			handle_error("recvfrom");
		}
		else { 
			
			sprintf(client_queresponse, "%s\n%d", inet_ntoa(client_address.sin_addr), client_address.sin_port);
			//printf("SERVER: The client sent the data: %s\n", client_response);
			// получить самое старое сообщение с наивысшим приоритетом
			if (mq_send (qd_server, client_queresponse, strlen (client_queresponse) + 1, 0) == -1) {
				perror ("Server: Not able to send message to client");
			}
			printf ("Server: response sent to client.\n"); 
		}
		
		//sendto(fd, port, sizeof(port), 0, (struct sockaddr *) &client_address, client_addr_size); //последний аргумент без указателя - ругается компилятор?????????????
					
		printf("SERVER IP address: %s\n", inet_ntoa(server_address.sin_addr));
		printf("SERVER port      : %d\n", ntohs(server_address.sin_port));
		
		printf("CLIENT IP address: %s\n", inet_ntoa(client_address.sin_addr));
		printf("CLIENT port      : %d\n", ntohs(client_address.sin_port));
	}

	for (i = 0; i < NUM_OF_THREADS; i++) {
		//ожидание завершения потоков
		status2 = pthread_join(serv_threads[i], NULL); 
		if (status2 != 0){
			printf("SERVER: main error: can't detach thread, status = %d\n", status2);
			handle_error("pthread_detach");
		}		
	}


	if (mq_close (qd_server) == -1) {
        perror ("Server: mq_close");
        exit (1);
    }

    if (mq_unlink (SERVER_QUEUE_NAME) == -1) {
        perror ("Server: mq_unlink");
        exit (1);
    }
	pthread_mutex_destroy(&mutex1);
	//pthread_mutex_destroy(&mutex2);
	close(fd);

	/* Код для работы с входящими соединениями... */

	/* Если больше не требуется, путь к сокету MY_SOCK_PATH следует удалить с помощью unlink(2) или remove(3) */
	return 0;
}


