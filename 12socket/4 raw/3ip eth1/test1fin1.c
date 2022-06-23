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
//#include <netinet/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <net/if.h>



//СДЕЛАТЬ RECVFROM SENDTO
#define LISTEN_BACKLOG 5
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

uint16_t checksum(struct iphdr ippart)
{
	int csum = 0, fsum = 0;
	short *ptr = (short *)&ippart;
	for(int i = 0; i < 10; i++)
	{
		csum += *ptr;
		ptr++;
	}
	csum = (csum >> 16) + (csum & 0xFFFF);
	fsum = ~csum;
	return (fsum & 0xFFFF);
}

struct message {
    struct ethhdr ethpart;
    struct iphdr ippart;
	struct udphdr udppart;
	char datapart[256];
};

int main(){

	char strmsg[] = "Hello server!\n\0"; //отправляемое сообщение
	char buf[128] = {0};
	
	struct sockaddr_ll server_address, server_address2;
	memset (&server_address, 0, sizeof (struct sockaddr_ll));
	memset (&server_address2, 0, sizeof (struct sockaddr_ll));

	unsigned char dest_mac[ETH_ALEN] = { 0x00, 0x0C, 0x29, 0x14, 0x7A, 0xD0 };  // 00:0C:29:14:7A:D0
	unsigned char src_mac[ETH_ALEN] = { 0x00, 0x0C, 0x29, 0x1D, 0xD6, 0x1E };  // 00:0C:29:1D:D6:1E

	server_address.sll_family = AF_PACKET;
	server_address.sll_ifindex = if_nametoindex ("ens38"); //Индекс сетевой карты по имени
	server_address.sll_halen = ETH_ALEN; //Сколько байт длина - 6
	memcpy (server_address.sll_addr, dest_mac, ETH_ALEN);
	
	
	char *dataping;
	struct message msg;
	memset (&msg, 0, sizeof (struct message));
	strcpy(msg.datapart, strmsg); //data
	
	//Заголовок Ethernet 
	/* destination eth addr	00:0C:29:14:7A:D0*/
	memcpy (msg.ethpart.h_dest, dest_mac, ETH_ALEN);
	/* source ether addr	 00:0C:29:1D:D6:1E*/
	memcpy (msg.ethpart.h_source, src_mac, ETH_ALEN);
	msg.ethpart.h_proto =  htons ( ETH_P_IP );	/* packet type ID field	*/
	
	//Заголовок IP
	msg.ippart.saddr = inet_addr("192.168.1.7"); //source ip
    msg.ippart.daddr = inet_addr("192.168.1.8"); //dest ip
	//не работает подсчет суммы адекватно, посмотрел сколько надо байт и вручную
	int size_ippart = sizeof(msg.ippart) + sizeof(msg.udppart) + strlen(strmsg); //htons(42); //sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(strmsg); 
    printf("tot_len: %d\n",size_ippart);

	msg.ippart.tot_len = size_ippart;
    msg.ippart.protocol = IPPROTO_UDP;
    msg.ippart.ttl = 255;
    msg.ippart.ihl = 5;
    msg.ippart.version = 4;
    msg.ippart.id = htons(54321);
    msg.ippart.tos = 0;
    msg.ippart.frag_off = 0;
    msg.ippart.check = checksum(msg.ippart);
	printf ("check: %02x\n", msg.ippart.check);

	//Заголовок UDP
	msg.udppart.source = htons(6666);
	msg.udppart.dest = htons(9002);
	msg.udppart.len = htons(8 + strlen(msg.datapart));//(sizeof(struct udphdr) + sizeof(data));//(8 + strlen(data));	 //tcp header size
	msg.udppart.check = 0;

	socklen_t server_addr_size = sizeof(struct sockaddr_ll); //размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.
	socklen_t server2_addr_size = sizeof(struct sockaddr_ll);

	int sizeofdata = sizeof(msg.ethpart)+ sizeof(msg.ippart)+ sizeof(msg.udppart) + strlen(msg.datapart);


	int fd;
	fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); //создание сервер сокета
	if (fd == -1)
		handle_error("socket");
	printf("Сокет создан\n");
	


	sendto(fd, &msg, sizeofdata, 0, (struct sockaddr *) &server_address, server_addr_size);
	printf ("Сообщение отправлено\n");

	char *data;
	char server_response[128]; //буфер принимаемого сообщения(отклик клиента)
	memset (server_response, 0, 128);

	while(1){
		//20 байт перепрыгнуть. смотреть на порт назначения с нашим портом. если совпал ответ от сервера
		int bytes_read = recvfrom (fd, &server_response, sizeof(server_response), 0, (struct sockaddr *) &server_address2, &server2_addr_size);
		if(bytes_read <= 0) {
			handle_error("recvfrom");
			printf ("Error: Nothing sent\n");
		}
		if (server_response != 0){

			struct iphdr *iph = (struct iphdr *) server_response;
			struct udphdr *udph = (struct udphdr *) (server_response + sizeof(struct iphdr));		
			//recv(fd, &datagram, sizeof(datagram), 0);
			data = server_response + sizeof (struct iphdr) + sizeof (struct udphdr) + sizeof (struct ether_header);
			
			uint32_t in_addr = INADDR_ANY;
			if ( (memcmp (server_address2.sll_addr, src_mac, ETH_ALEN) == 0) && ( (iph -> saddr) == in_addr) && ( ntohs(udph -> dest) == 6666 ) ) {
				printf ("The server sent the data: %s\n", data);
				break;
			}
		}
		else { printf ("server_response = 0\n"); }
		
	}

	
	close (fd);

	return 0;
}



