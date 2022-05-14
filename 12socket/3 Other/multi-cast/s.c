//MULTICAST
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

	char s_message[50] = "MULTICAST в один конец!\n"; //отправляемое сообщение(от сервера)
	char client_response[256]; //буфер принимаемого сообщения(отклик клиента)
	
	//указать адрес для сокета
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(struct sockaddr_in)); /*Зануление структуры, очистка*/
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	//server_address.sin_addr.s_addr = INADDR_ANY;
	inet_aton("224.0.0.1", &server_address.sin_addr); //до 239.255.255.255 - multicast

	/*
	strncpy(server_address.sin_path, MY_SOCK_PATH, sizeof(server_address.sin_path) - 1); //защита от переполнения
	server_address.sin_path[sizeof(server_address.sin_path) - 1] = '\0'; //зануление последнего байта чтобы не потерять данные
	*/
		
	int fd;
	fd = socket(AF_INET, SOCK_DGRAM, 0); //создание сервер сокета
	if (fd == -1)
		handle_error("socket");
	     
        
	socklen_t server_addr_size = sizeof(struct sockaddr_in); /*размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.*/	
	//ВОЗМОЖНО ВМЕСТО sockaddr_in НАДО client_address
	
	sendto(fd, s_message, sizeof(s_message), 0, (struct sockaddr *) &server_address, server_addr_size); //последний аргумент без указателя - ругается компилятор?????????????
		
		
	//НА САМОМ ДЕЛЕ НЕ НАШ, А ТОТ НА КОТОРЫЙ ЗАБИНДЕН СОКЕТ
	//КАК УЗНАТЬ НАШ?	
	printf("SERVER IP address: %s\n", inet_ntoa(server_address.sin_addr));
	printf("SERVER port      : %d\n", ntohs(server_address.sin_port));
	
	
	close(fd);


	return 0;
}


