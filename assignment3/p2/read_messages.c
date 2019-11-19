#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>


#define LOG_PACKET_SEP "----------------------------------------------\n"
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

	int socket;
	int saddr_len;
	struct sockaddr saddr;
	char buf[BUF_LEN];
	

	if((socket=socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		perror("socket(): ");	
		exit(EXIT_FAILURE);
	}


	printf("Press Ctrl+C to exit...\n");

	while(true) {
		saddr_len = sizeof(saddr);	
		int buf_len_act;
		if((buf_len_act=recvfrom(socket, buf, BUF_LEN, 0, &saddr, &saddr_len)) < 0) {
			perror("recvfrom(): ");	
			exit(EXIT_FAILURE);
		}

		printf(LOG_PACKET_SEP);

		int excl_len = 0;
		struct ethhdr *eth = (struct ethhdr*)(buffer + excl_len);
		excl_len += sizeof(struct ethhdr);		
		printf("Source Mac Address: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
		printf("Destination Mac Address: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x", eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);

		printf();
		
		struct iphdr *ip = (struct iphdr*)(buffer + excl_len);
		int ip_hdr_len = ip->ihl << 2;
		excl_len += sizeof(struct iphdr);

		printf(LOG_PACKET_SEP);
		printf("\n");
