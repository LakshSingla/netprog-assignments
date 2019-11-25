#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main () {
	struct sockaddr_in addr;

	int sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock < 0) {
		perror("error with socket()");
		exit(0);
	}

	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(6000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	/*addr.sin_addr.s_addr = inet_addr("172.17.0.1");*/

	int addr_len = sizeof(addr);

	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("error with bind()");
		exit(0);
	}

	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr("239.0.0.5");
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		perror("error with setsockopt()");
		exit(0);
	}

	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = inet_addr("239.0.0.6");
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		perror("error with setsockopt()");
		exit(0);
	}

	while (1) {
		char msg[1024];
		int c = recvfrom(sock, msg, sizeof(msg), 0, (struct sockaddr *) &addr, &addr_len);
		if (c < 0) {
			perror("error with recvfrom()");
			exit(0);
		}
		printf("%s: msg = %s\n\n", inet_ntoa(addr.sin_addr), msg);
	}
}
