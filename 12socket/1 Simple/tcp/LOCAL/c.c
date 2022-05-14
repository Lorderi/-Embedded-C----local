#include <unistd.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MY_SOCK_PATH "/tmp/stream_server"
#define LISTEN_BACKLOG 5
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main(){

	char c_message[32] = "Hello server from client!\n"; //отправляемое сообщение(от сервера)
	char server_response[256]; //буфер принимаемого сообщения(отклик клиента)
	
	//указать адрес для сокета
	struct sockaddr_un server_address;
	memset(&server_address, 0, sizeof(struct sockaddr_un)); /*Зануление структуры, очистка*/
	server_address.sun_family = AF_LOCAL;
	//client.sun_path = MY_SOCK_PATH;
	strncpy(server_address.sun_path, MY_SOCK_PATH, sizeof(server_address.sun_path) - 1); //защита от переполнения
	server_address.sun_path[sizeof(server_address.sun_path) - 1] = '\0'; //зануление последнего байта чтобы не потерять данные
	
	
	int fd;
	fd = socket(AF_LOCAL, SOCK_STREAM, 0); //создание сервер сокета
	if (fd == -1)
		handle_error("socket");
		
	// установления связи с сервером
	int connection_status = connect(fd, (struct sockaddr *) &server_address, sizeof(server_address));
	if (connection_status == -1){
		handle_error("Connect");
	}
	
	send(fd, c_message, sizeof(c_message), 0);
	recv(fd, &server_response, sizeof(server_response), 0);
	
	if (server_response != NULL)
		printf("The server sent the data: %s\n", server_response);
	else printf ("Error: Nothing sent\n");
	close(fd);
	/* Код для работы с входящими соединениями... */

	/* Если больше не требуется, путь к сокету MY_SOCK_PATH следует удалить с помощью unlink(2) или remove(3) */
	return 0;
}



