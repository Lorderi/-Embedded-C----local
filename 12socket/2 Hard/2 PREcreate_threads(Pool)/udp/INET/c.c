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

	char c_message[50] = "Hello main_server from client!\n"; //отправляемое сообщение(от сервера)
	char c2_message[50] = "Hello subserver from client!\n"; //отправляемое сообщение(от сервера)
	char subserver_response[256]; //буфер принимаемого сообщения(отклик клиента)
	
	//указать адрес для сокета
	struct sockaddr_in server_address, subserver_address;
	memset(&server_address, 0, sizeof(struct sockaddr_in)); /*Зануление структуры, очистка*/
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
		
	int fd;
	fd = socket(AF_INET, SOCK_DGRAM, 0); //создание сервер сокета
	if (fd == -1)
		handle_error("socket");
	/*	
	// установления связи с сервером
	int connection_status = connect(fd, (struct sockaddr *) &server_address, sizeof(server_address));
	if (connection_status == -1){
		handle_error("Connect");
	}
	
	send(fd, c_message, sizeof(c_message), 0);
	recv(fd, &server_response, sizeof(server_response), 0);
	*/
	//привязать сокет к указанному нами IP и порту
	//if (bind(fd, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in)) == -1)
	//	handle_error("bind");
		
	socklen_t server_addr_size = sizeof(struct sockaddr_in); /*размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.*/	
	socklen_t subserver_addr_size = sizeof(struct sockaddr_in); /*размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.*/	
	//ВОЗМОЖНО ВМЕСТО sockaddr_in НАДО client_address
	
	
	sendto(fd, c_message, sizeof(c_message), 0, (struct sockaddr *) &server_address, server_addr_size); //последний аргумент без указателя - ругается компилятор?????????????
	
	
	int bytes_read = recvfrom(fd, &subserver_response, sizeof(subserver_response), 0, (struct sockaddr *) &subserver_address, &subserver_addr_size);
	if(bytes_read <= 0) {
		handle_error("recvfrom");
		printf ("Error: Nothing sent\n");
	}
	else printf("The subserver sent the data: %s\n", subserver_response);
	
	
	sendto(fd, c2_message, sizeof(c2_message), 0, (struct sockaddr *) &subserver_address, subserver_addr_size); //последний аргумент без указателя - ругается компилятор?????????????
			
		
	printf("SERVER IP address(ntoa): %s\n", inet_ntoa(server_address.sin_addr));
	printf("SERVER port     (ntons): %d\n", ntohs(server_address.sin_port));
	printf("SUBSERVER IP address(ntoa): %s\n", inet_ntoa(subserver_address.sin_addr));
	printf("SUBSERVER port     (ntons): %d\n", ntohs(subserver_address.sin_port));
	
	struct sockaddr_in monitor_caddress;
	
	socklen_t monitor_caddress_addr_size = sizeof(struct sockaddr_in);
	getsockname(fd, (struct sockaddr*) &monitor_caddress, &monitor_caddress_addr_size);
	
	printf("CLIENT IP address(ntoa): %s\n", inet_ntoa(monitor_caddress.sin_addr));
	printf("CLIENT port     (ntohs): %d\n", ntohs(monitor_caddress.sin_port));
	
	
	close(fd);
	/* Код для работы с входящими соединениями... */

	/* Если больше не требуется, путь к сокету MY_SOCK_PATH следует удалить с помощью unlink(2) или remove(3) */
	return 0;
}



