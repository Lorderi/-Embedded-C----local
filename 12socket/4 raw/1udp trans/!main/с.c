#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/udp.h>	//Provides declarations for udp header
#include <netinet/ip.h>	//Provides declarations for ip header
#include <arpa/inet.h>
//СДЕЛАТЬ RECVFROM SENDTO
#define LISTEN_BACKLOG 5
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

//20 байт айпи заголовок
//12 байт заголовок udp
/*
typedef struct udps {
	short source;
	short dest;
	short length;
	short checksum;
} udparg_t;
*/
int main(){
	
	//char c_message[32] = "Hello server from client!\n"; //отправляемое сообщение(от сервера)
	char server_response[4096]; //буфер принимаемого сообщения(отклик клиента)
	
	//указать адрес для сокета
	struct sockaddr_in server_address, server2_addr;
	memset(&server_address, 0, sizeof(struct sockaddr_in)); /*Зануление структуры, очистка*/
	memset(&server2_addr, 0, sizeof(struct sockaddr_in)); /*Зануление структуры, очистка*/
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;
	/*
	strncpy(server_address.sin_path, MY_SOCK_PATH, sizeof(server_address.sin_path) - 1); //защита от переполнения
	server_address.sin_path[sizeof(server_address.sin_path) - 1] = '\0'; //зануление последнего байта чтобы не потерять данные
	*/
	
	int fd;
	fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP); //создание сервер сокета
	if (fd == -1)
		handle_error("socket");
	printf("Сокет создан\n");
	// установления связи с сервером

/*
	int connection_status = connect(fd, (struct sockaddr *) &server_address, sizeof(server_address));
	if (connection_status == -1){
		handle_error("Connect");
	}*/

	//printf("Соединение установлено\n");
	//udparg_t udp;
	

	char datagram[4096], *data, strport[2];
	//zero out the packet buffer
	memset (datagram, 0, 4096);
	memset (server_response, 0, 4096);
	//UDP header
	struct udphdr *udppacket = (struct udphdr *) datagram;

	//Data part
	data = datagram + sizeof(struct udphdr);
	strcpy(data , "Hello server!\n\0");

	udppacket->source = htons (6666);
	udppacket->dest = server_address.sin_port;
	udppacket->len = htons (8 + strlen(data));//(sizeof(struct udphdr) + sizeof(data));//(8 + strlen(data));	 //tcp header size
	udppacket->check = 0;
	
	socklen_t server_addr_size = sizeof(struct sockaddr_in); //размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.
	socklen_t server2_addr_size = sizeof(struct sockaddr_in);
	
	sendto(fd, datagram, sizeof(struct udphdr) + strlen(data), 0, (struct sockaddr *) &server_address, server_addr_size);
	//send(fd, datagram, sizeof(udparg_t) + strlen(data), 0);
	printf("Сообщение отправлено\n");
	int iport;
	
	
	
	while(1){
		//20 байт перепрыгнуть. смотреть на порт назначения с нашим портом. если совпал ответ от сервера
		int bytes_read = recvfrom(fd, &server_response, sizeof(server_response), 0, (struct sockaddr *) &server_address, &server_addr_size);
		if(bytes_read <= 0) {
			handle_error("recvfrom");
			printf ("Error: Nothing sent\n");
		}
		if (server_response != 0){
			struct iphdr *iph = (struct iphdr *) server_response;
			struct udphdr *udph = (struct udphdr *) (server_response + sizeof(struct iphdr));		
			//recv(fd, &datagram, sizeof(datagram), 0);
			data = server_response + sizeof(struct iphdr) + sizeof(struct udphdr);
			/*
			printf("data:%s\n", data);
			strncpy(strport, data, 2);
			printf("strport:%s\n", strport);
			iport = atoi(strport);
			printf("data:%d\n", iport);
			printf("ntohs data:%d\n", ntohs(iport));
			*/
			//printf("ntohs(udph->dest):%d\n", ntohs(udph->dest));
			
			/*
			char addr1[32], addr2[32];
			sprintf(addr1, "%d", server_address.sin_addr.s_addr);
			sprintf(addr2, "%d", iph->saddr);
			*/ //(strcmp(addr1, addr2)==0)
			if((ntohs(udph->dest) == 6666)&&(server_address.sin_addr.s_addr == iph->saddr)){
				printf("The server sent the data: %s\n", data);
				break;
			}
		}
		else{printf("server_response = 0\n");}
		
	}
	/*
	socklen_t server_addr_size = sizeof(struct sockaddr_in); /*размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.
	socklen_t server2_addr_size = sizeof(struct sockaddr_in);
	
	sendto(fd, datagram, sizeof(udparg_t) + strlen(data), 0, (struct sockaddr *) &server_address, server_addr_size); //последний аргумент без указателя - ругается компилятор?????????????
	
	
	int bytes_read = recvfrom(fd, &datagram, sizeof(datagram), 0, (struct sockaddr *) &server2_addr, &server2_addr_size);
	if(bytes_read <= 0) {
		handle_error("recvfrom");
		printf ("Error: Nothing sent\n");
	}
	else printf("The subserver sent the data: %s\n", subserver_response);
	*/
	
	printf("SERVER IP address(ntoa): %s\n", inet_ntoa(server_address.sin_addr));
	printf("SERVER port     (ntons): %d\n", ntohs(server_address.sin_port));
	
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



