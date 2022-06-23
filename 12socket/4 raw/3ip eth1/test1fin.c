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
#include <net/ethernet.h>
#include <net/if.h>



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
/*
uint16_t checksum(uint16_t *buffer, int size)
{
    int cksum=0;
    while (size > 1){
        cksum += *buffer++;
        size  -= sizeof(uint16_t);  
    }
    if (size){
        cksum += *(uint16_t*)buffer;  
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (uint16_t)(~cksum);
}
*/
/*
uint16_t checksum(uint16_t *buffer)
{
    int csum = 0, fincsum = 0;
	int tmp = 0;

	for (int i = 0; i < 10; i++){
		csum += *buffer;
		buffer++;
	}
	tmp = csum >> 16;
	csum = (csum & 0xFFFF) + tmp;
    return (uint16_t)(~csum);
}*/

unsigned short checksum(unsigned short *buf,int nword)
{
	unsigned long sum;
	for(sum=0;nword>0;nword--)
	{
		sum += *buf++;
		sum = (sum>>16) + (sum&0xffff);
	}
	return ~sum;
}

int main (){
	/*строки ip*/
	char source_ip[32], dest_ip[32];
	strcpy (source_ip , "192.168.1.7");
	strcpy (dest_ip , "192.168.1.8");
	char server_response[128]; //буфер принимаемого сообщения(отклик клиента)
	char strmsg[] = "Hello server!\n\0"; //отправляемое сообщение

	unsigned char dest_mac[ETH_ALEN] = { 0x00, 0x0C, 0x29, 0x14, 0x7A, 0xD0 };  // 00:0C:29:14:7A:D0 mac addr...
	unsigned char src_mac[ETH_ALEN] = { 0x00, 0x0C, 0x29, 0x1D, 0xD6, 0x1E };  // 00:0C:29:1D:D6:1E mac addr...

	struct sockaddr_ll server_address, server_address2;
	memset (&server_address, 0, sizeof (struct sockaddr_ll));
	memset (&server_address2, 0, sizeof (struct sockaddr_ll));
	server_address.sll_family = AF_PACKET;
	server_address.sll_ifindex = if_nametoindex ("ens38"); //Индекс сетевой карты по имени
	server_address.sll_halen = ETH_ALEN; //Сколько байт длина - 6
	
	/*
	server_address.sll_addr[0] = 0x00;
	server_address.sll_addr[1] = 0x0C;
	server_address.sll_addr[2] = 0x29;
	server_address.sll_addr[3] = 0x14;
	server_address.sll_addr[4] = 0x7A;
	server_address.sll_addr[5] = 0xD0;
	server_address.sll_addr[6] = 0x00;
	server_address.sll_addr[7] = 0x00;
	*/
	memcpy (server_address.sll_addr, dest_mac, ETH_ALEN);
	server_address.sll_addr[6] = 0x00;
	server_address.sll_addr[7] = 0x00;
	

	int fd;
	fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); //создание сервер сокета
	if (fd == -1)
		handle_error("socket");
	printf("Сокет создан\n");

	
	char datagram[42], *data;// strport[2];
	//зануляем буфферы
	memset (datagram, 0, 42);
	memset (server_response, 0, 128);

	//ethernet header
	struct ether_header * ehdr = (struct ether_header *) datagram;
	//IP header
	struct iphdr *ippacket = (struct iphdr *) (datagram + sizeof(struct ether_header));
	//UDP header
	struct udphdr *udppacket = (struct udphdr *) (datagram + sizeof(struct ether_header) + sizeof(struct iphdr));
	


	//Data part
	data = datagram + sizeof(struct udphdr) + sizeof(struct iphdr) + sizeof(struct ether_header);
	strcpy (data , strmsg);

	
	//Заголовок Ethernet 
	/* destination eth addr	00:0C:29:14:7A:D0*/
	memcpy (ehdr -> ether_dhost, dest_mac, ETH_ALEN);
	/*
	ehdr -> ether_dhost[0] = 0x00;
	ehdr -> ether_dhost[1] = 0x0C;
	ehdr -> ether_dhost[2] = 0x29;
	ehdr -> ether_dhost[3] = 0x14;
	ehdr -> ether_dhost[4] = 0x7A;
	ehdr -> ether_dhost[5] = 0xD0;
	*/
	
	
	/* source ether addr	 00:0C:29:1D:D6:1E*/
	memcpy (ehdr -> ether_shost, src_mac, ETH_ALEN);
	
	/*
	ehdr -> ether_shost[0] = 0x00;
	ehdr -> ether_shost[1] = 0x0C;
	ehdr -> ether_shost[2] = 0x29;
	ehdr -> ether_shost[3] = 0x1D;
	ehdr -> ether_shost[4] = 0xD6;
	ehdr -> ether_shost[5] = 0x1E;
	*/
	
	ehdr -> ether_type =  htons ( ETH_P_IP );		        /* packet type ID field	*/
	
	//Заголовок IP
	ippacket -> saddr = inet_addr (source_ip);
    ippacket -> daddr = inet_addr (dest_ip);
	//не работает подсчет суммы адекватно, посмотрел сколько надо байт и вручную
	int sssize = htons(42); //sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(strmsg);
    printf("tot_len: %d\n",sssize);

	ippacket -> tot_len = sssize;
    ippacket -> protocol = IPPROTO_UDP;
    ippacket -> ttl = 255;
    ippacket -> ihl = 5;
    ippacket -> version = 4;
    ippacket -> id = htons(54321);
    ippacket -> tos = 0;
    ippacket -> frag_off = 0;
    ippacket -> check = 0;

	//Заголовок UDP
	udppacket -> source = htons (6666);
	udppacket -> dest = htons(9002);
	udppacket -> len = htons (8 + strlen(data));//(sizeof(struct udphdr) + sizeof(data));//(8 + strlen(data));	 //tcp header size
	udppacket -> check = 0;


	int csum = 0, fincsum = 0;
	uint16_t * ptr = (uint16_t *) &ippacket;
	int tmp = 0;
	
	/*
	for (int i = 0; i < 10; i++){
		csum += *ptr;
		ptr++;
	}
	tmp = csum >> 16;
	csum = (csum & 0xFFFF) + (csum >> 16);
	/*tmp = csum >> 16;
	csum = (csum & 0xFFFF) + tmp;
	*/

	fincsum = ~csum;
	printf ("check: %02x\n", fincsum & 0xFFFF);
	
	
	ippacket -> check = htons(fincsum & 0xFFFF); //htons(fincsum & 0xFFFF);
	ippacket -> check = htons(checksum(ptr, 20));
	printf ("check: %02x\n", ippacket -> check);
	
	
	
	socklen_t server_addr_size = sizeof(struct sockaddr_ll); //размер (в байтах) структуры, на которую указывает addr; по возвращении он будет содержать фактический размер адреса равноправного узла.
	socklen_t server2_addr_size = sizeof(struct sockaddr_ll);
	
	sendto (fd, datagram, sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(strmsg), 0, (struct sockaddr *) &server_address, server_addr_size);
	//send(fd, datagram, sizeof(udparg_t) + strlen(data), 0);
	printf ("Сообщение отправлено\n");
	//int iport;
	
	
	
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



