#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#define NUM_OF_THREADS 5
#define NUM_PORT 9002

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)
//Почему то всегда с 9003 порта сабсервер, несмотря на то что создан пул портов. Возможно потому что обрабатываются запуски клиентов последовательно, а не параллельно
//pthread_cond_wait pthread_cond_signal в месте где в сервисе инициализируется через указатель структура, чтобы она успевала проинициализироваться
 
int port_table[256] = {0}; //таблица портов 
int num_port = NUM_PORT + 1;
pthread_mutex_t mutex1;
pthread_cond_t c1 = PTHREAD_COND_INITIALIZER;
/*
typedef struct serv_tag {
	int port_id;
} servarg_t;
*/

void* service(void *args) {
	/*инициализируем структуру локальной функции*/
	struct sockaddr_in *arg = (struct sockaddr_in *) args;
	//servarg_t *arg = (servarg_t*)args; //переданные аргументы в функцию
	//port = arg->port_id;
	struct sockaddr_in subserver_address, client_address;
	//pthread_mutex_lock(&mutex2);
	client_address = *arg;
	//pthread_mutex_unlock(&mutex2);
	//pthread_cond_signal(&c1);
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
	
	memset(&subserver_address, 0, sizeof(struct sockaddr_in)); /*Зануление структуры, очистка*/
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
		
	//привязать сокет к указанному нами IP и порту
	//if (bind(fd, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in)) == -1)
	//	handle_error("subserver bind");

	// второй агрумент это бэклог - сколько соединений может одновременно ожидать этот сокет
	
	socklen_t client_addr_size = sizeof(struct sockaddr_in); /*размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.*/	

	//ВОЗМОЖНО ВМЕСТО sockaddr_in НАДО client_address
	
	sendto(fd, s_message, sizeof(s_message), 0, (struct sockaddr *) &client_address, client_addr_size); //последний аргумент без указателя - ругается компилятор?????????????
	
	
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
	close(fd);
	pthread_mutex_lock(&mutex1);
	port_table[i] = 0;
	pthread_mutex_unlock(&mutex1);
}




int main(){

	/* ЗАФИГАЧИТЬ КОГДА БУДУТ ОБРАЩАТЬСЯ К ТАБЛИЦЕ
	pthread_mutex_lock(&mutex)
	pthread_mutex_unlock(&mutex);
	*/

	int status1, status2; //статус
	/*Т.к. мы не знаем сколько будет потоков, т.к. в нашей схеме оно образуется по мере пополнения, то выставляем произвольную цифру, на которой в конце цикла, при дохождении до 255 будет обнуление. Сделано это потому что динамику пока что не знаю, надо по факту сделать динамический.
	В данном решении задачи могут возникнуть проблемы, т.к. если дойдет до 255, а поток с 0 будет всё так же занят то пойдет коллизия, и id потоков не будут присваиваться*/
	
	pthread_t serv_threads[256]; //id потоков
	int i = 0; //номера в id потоков serv_threads. проходят до 255 и обнуляются. Для id потоков
	
	char client_response[256]; //буфер принимаемого сообщения(отклик клиента)
	
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

	
	socklen_t client_addr_size = sizeof(struct sockaddr_in); /*размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.*/	
	
	//ВОЗМОЖНО ВМЕСТО sockaddr_in НАДО client_address
	
	if ((pthread_mutex_init(&mutex1, NULL)) != 0){
			handle_error("mutex1_init");
		}
		/*
	if ((pthread_mutex_init(&mutex2, NULL)) != 0){
			handle_error("mutex2_init");
		}
	*/
	for(;;) {
		//if (i == 0){
		//bytes_read = recvfrom(fd, &client_response, sizeof(client_response), 0, (struct sockaddr *) &client_address, &client_addr_size);
		//}
		/*
		else {
			pthread_mutex_lock(&mutex2);
			pthread_cond_wait(&c1, &mutex2);
			bytes_read = recvfrom(fd, &client_response, sizeof(client_response), 0, (struct sockaddr *) &client_address, &client_addr_size);
			pthread_mutex_unlock(&mutex2);
		}
		*/
		bytes_read = recvfrom(fd, &client_response, sizeof(client_response), 0, (struct sockaddr *) &client_address, &client_addr_size);
		if(bytes_read <= 0) {
			printf ("SERVER: Error: Nothing sent, code :%d\n", bytes_read);
			handle_error("recvfrom");
		}
		else printf("SERVER: The client sent the data: %s\n", client_response);
				
		//sendto(fd, port, sizeof(port), 0, (struct sockaddr *) &client_address, client_addr_size); //последний аргумент без указателя - ругается компилятор?????????????
		//создание потоков
		status1 = pthread_create(&serv_threads[i], NULL, service, &client_address);
		if (status1 != 0) {
			printf("SERVER: main error: can't create thread, status = %d\n", status1);
			handle_error("pthread_create");
		}
		
		
		
		//ожидание завершения потоков
		status2 = pthread_detach(serv_threads[i]); 
		if (status2 != 0){
			printf("SERVER: main error: can't detach thread, status = %d\n", status2);
			handle_error("pthread_detach");
		}
				
		i++;
			
				
		printf("SERVER IP address: %s\n", inet_ntoa(server_address.sin_addr));
		printf("SERVER port      : %d\n", ntohs(server_address.sin_port));
		
		printf("CLIENT IP address: %s\n", inet_ntoa(client_address.sin_addr));
		printf("CLIENT port      : %d\n", ntohs(client_address.sin_port));


		if (i == 255){
			i = 0;
		}
	}
	pthread_mutex_destroy(&mutex1);
	//pthread_mutex_destroy(&mutex2);
	close(fd);

	/* Код для работы с входящими соединениями... */

	/* Если больше не требуется, путь к сокету MY_SOCK_PATH следует удалить с помощью unlink(2) или remove(3) */
	return 0;
}


