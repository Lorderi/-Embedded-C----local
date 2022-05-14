#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#define LISTEN_BACKLOG 5
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main(){

	char s_message[32] = "Hello client from server!\n"; //отправляемое сообщение(от сервера)
	char client_response[256]; //буфер принимаемого сообщения(отклик клиента)
	
	//указать адрес для сокета
	struct sockaddr_in server_address, client_address;
	memset(&server_address, 0, sizeof(struct sockaddr_in)); /*Зануление структуры, очистка*/
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;
	/*
	strncpy(server_address.sin_path, MY_SOCK_PATH, sizeof(server_address.sin_path) - 1); //защита от переполнения
	server_address.sin_path[sizeof(server_address.sin_path) - 1] = '\0'; //зануление последнего байта чтобы не потерять данные
	*/
		
	int fd;
	fd = socket(AF_INET, SOCK_DGRAM, 0); //создание сервер сокета
	if (fd == -1)
		handle_error("socket");
		
		
	//привязать сокет к указанному нами IP и порту
	if (bind(fd, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in)) == -1)
		handle_error("bind");

	// второй агрумент это бэклог - сколько соединений может одновременно ожидать этот сокет
	/*
	if (listen(fd, LISTEN_BACKLOG) == -1)
		handle_error("listen");
	//Теперь мы можем принимать входящие подключения по одному за один раз, используя accept(2)
	socklen_t client_addr_size = sizeof(struct sockaddr_in); //размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.
	int client_fd;

	
	client_fd = accept(fd, (struct sockaddr *) &client_address, &client_addr_size);
	
	if (client_fd  == -1)
		handle_error("accept");
		
	*/
	socklen_t client_addr_size = sizeof(struct sockaddr_in); /*размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.*/	
	
	//-----------------------------------------------
	//ВОЗМОЖНО ВМЕСТО sockaddr_in НАДО client_address
	

//	while (1) {
		int bytes_read = recvfrom(fd, &client_response, sizeof(client_response), 0, (struct sockaddr *) &client_address, &client_addr_size);
		if(bytes_read <= 0) {
			handle_error("recvfrom");
			printf ("Error: Nothing sent\n");
//			break;
		}
		else printf("The client sent the data: %s\n", client_response);
		sendto(fd, s_message, sizeof(s_message), 0, (struct sockaddr *) &client_address, client_addr_size); //последний аргумент без указателя - ругается компилятор?????????????
		/*
		if (client_response != NULL)
			printf("The client sent the data: %s\n", client_response);
		else printf ("Error: Nothing sent\n");*/
//	}
	

	close(fd);

	/* Код для работы с входящими соединениями... */

	/* Если больше не требуется, путь к сокету MY_SOCK_PATH следует удалить с помощью unlink(2) или remove(3) */
	return 0;
}


