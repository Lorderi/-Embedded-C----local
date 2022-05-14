#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LISTEN_BACKLOG 5
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main(){

	char server_response[256]; //буфер принимаемого сообщения(отклик клиента)
	
	//указать адрес для сокета
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(struct sockaddr_in)); /*Зануление структуры, очистка*/
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	//server_address.sin_addr.s_addr = INADDR_ANY;
	inet_aton("255.255.255.255", &server_address.sin_addr);
	/*
	strncpy(server_address.sin_path, MY_SOCK_PATH, sizeof(server_address.sin_path) - 1); //защита от переполнения
	server_address.sin_path[sizeof(server_address.sin_path) - 1] = '\0'; //зануление последнего байта чтобы не потерять данные
	*/
	
	int fd;
	fd = socket(AF_INET, SOCK_DGRAM, 0); //создание сервер сокета
	if (fd == -1)
		handle_error("socket");
	if (bind(fd, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in)) == -1)
		handle_error("bind");	
	
	
	socklen_t server_addr_size = sizeof(struct sockaddr_in); /*размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.*/	
	//ВОЗМОЖНО ВМЕСТО sockaddr_in НАДО client_address
	
	
	while(1){
		recvfrom(fd, &server_response, sizeof(server_response), 0, (struct sockaddr *) &server_address, &server_addr_size); //последний аргумент без указателя - ругается компилятор?????????????
//		recv(fd, &server_response, sizeof(server_response), 0); 
		
		if (server_response != NULL) {
			printf("The server sent the data: %s\n", server_response);
			break;
		}
		else printf ("Error: Nothing sent\n");
		
		
		
	}
	

	printf("SERVER IP address(ntoa): %s\n", inet_ntoa(server_address.sin_addr));
	printf("SERVER port     (ntons): %d\n", ntohs(server_address.sin_port));
	
	struct sockaddr_in monitor_caddress;
	
	socklen_t monitor_caddress_addr_size = sizeof(struct sockaddr_in);
	getsockname(fd, (struct sockaddr*) &monitor_caddress, &monitor_caddress_addr_size);
	//НА САМОМ ДЕЛЕ НЕ НАШ, А ТОТ НА КОТОРЫЙ ЗАБИНДЕН СОКЕТ
	//КАК УЗНАТЬ НАШ?	
	printf("CLIENT IP address(ntoa): %s\n", inet_ntoa(monitor_caddress.sin_addr));
	printf("CLIENT port     (ntohs): %d\n", ntohs(monitor_caddress.sin_port));
	
	
	close(fd);
	/* Код для работы с входящими соединениями... */

	/* Если больше не требуется, путь к сокету MY_SOCK_PATH следует удалить с помощью unlink(2) или remove(3) */
	return 0;
}



