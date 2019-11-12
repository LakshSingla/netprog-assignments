struct broker {
	char ip[INET_ADDRSTRLEN];
	int port;
	char n1_ip[INET_ADDRSTRLEN];
	int n1_port;
	char n2_ip[INET_ADDRSTRLEN];
	int n2_port;
};

struct broker BROKERS[3] = {
	{"127.0.0.1", 4000, "127.0.0.1", 5000, "127.0.0.1", 6000},
	{"127.0.0.1", 5000, "127.0.0.1", 4000, "127.0.0.1", 6000},
	{"127.0.0.1", 6000, "127.0.0.1", 5000, "127.0.0.1", 4000}
};
