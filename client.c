#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
	if (argc != 2) return -1;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct hostent *server;
	
	server = gethostbyname("localhost");
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host");
		exit(0);
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	server_addr.sin_port = htons(atoi(argv[1]));

	if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("connect");
	}

	char msg_buffer[BUFFER_SIZE];
	memset(msg_buffer, 0, BUFFER_SIZE);
	for(;;) {
		printf("Your message: ");
		fgets(msg_buffer, BUFFER_SIZE - 1, stdin);

		int res = write(sockfd, msg_buffer, BUFFER_SIZE);
		if (res <= 0) {
			break;
		}

		memset(msg_buffer, 0, BUFFER_SIZE);
	}

	close(sockfd);
}
