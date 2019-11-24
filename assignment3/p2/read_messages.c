#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/ethernet.h>
#include <netinet/ip.h>


#define LOG_PACKET_SEP "======================================================================================================\n"
#define BUF_LEN (1024*32)

FILE *fp;

void exit_handler_1(void) {
	fclose(fp);
}

int main(int argc, char **argv) {

	if(argc != 2) {
		printf("Usage: ./read_messages <log_file>\n");
		exit(EXIT_FAILURE);
	}

	if(atexit(exit_handler_1) != 0) {
		printf("Unable to establish the exit handler. File may not be logged properly\n");
	}

	if((fp=fopen(argv[1], "w")) == NULL) {
		printf("Unable to open the log_file: %s for writing\n", argv[1]);	
		exit(EXIT_FAILURE);
	}

	int sock;
	int saddr_len;
	struct sockaddr saddr;
	char buf[BUF_LEN];
	

	if((sock=socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		perror("socket(): ");	
		exit(EXIT_FAILURE);
	}


	printf("Press Ctrl+C to exit...\n");

	while(true) {
		saddr_len = sizeof(saddr);	
		int buf_len_act;
		if((buf_len_act=recvfrom(sock, buf, BUF_LEN, 0, &saddr, &saddr_len)) < 0) {
			perror("recvfrom(): ");	
			exit(EXIT_FAILURE);
		}

		printf(LOG_PACKET_SEP);

		int excl_len = 0;
		struct ethhdr *eth = (struct ethhdr*)(buf + excl_len);
		excl_len += sizeof(struct ethhdr);		
		printf("Source Mac Address: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x, ", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
		printf("Destination Mac Address: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);

		struct ip *iph = (struct ip*)(buf + excl_len);
		int ip_hdr_len = iph->ip_hl << 2;
		excl_len += ip_hdr_len;

		char src_ip_buf[100], dst_ip_buf[100];
		
		inet_ntop(AF_INET, &(iph->ip_src), src_ip_buf, 100);
		inet_ntop(AF_INET, &(iph->ip_dst), dst_ip_buf, 100);

		printf("Source IP address: %s, Destination IP address: %s\nIP TTL: %d, IP Protocol no.: %d\n", src_ip_buf, dst_ip_buf, iph->ip_ttl, iph->ip_p);

		void *transport_layer_ds = buf + excl_len;

		/*if(iph->ip_p == IPPROTO_ICMP) {
		
		}*/
		if(iph->ip_p == IPPROTO_TCP) {
			struct tcphdr *tcph = (struct tcphdr*) transport_layer_ds;
			printf("TCP Port Info | Source port: %d, Destination port: %d\n", ntohs(tcph->source), ntohs(tcph->dest));
			printf("TCP Packet Info | Window: %d, SYN: %d, FIN: %d, RST: %d, ACK: %d\n", ntohs(tcph->window), ntohs(tcph->syn), ntohs(tcph->fin),ntohs(tcph->rst),ntohs(tcph->ack));
		}
		else if(iph->ip_p == IPPROTO_UDP) {
			struct udphdr *udph = (struct udphdr*)transport_layer_ds;	
			printf("UDP Port Info | Source port: %d, Destination port: %d\n", ntohs(udph->source), ntohs(udph->dest));
		}

		printf(LOG_PACKET_SEP);
		printf("\n");
		

	}
}
