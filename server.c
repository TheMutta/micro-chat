#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#define BUFFER_SIZE 1024

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_addr, clientone_addr, clienttwo_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(1234);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
	}

	listen(sockfd, 5);

	socklen_t clionelen = sizeof(clientone_addr);
	socklen_t clitwolen = sizeof(clienttwo_addr);

	for(;;) {
		char msg_buffer[BUFFER_SIZE];
		memset(msg_buffer, 0, BUFFER_SIZE);

		int clientone = accept(sockfd, (struct sockaddr *)&clientone_addr, &clionelen);
		int clienttwo = accept(sockfd, (struct sockaddr *)&clienttwo_addr, &clitwolen);
	
		struct pollfd fds[2];
		fds[0].fd = clientone;
		fds[0].events = POLLIN;
		fds[1].fd = clienttwo;
		fds[1].events = POLLIN;

		for (;;) {
			int num_fds_ready = poll(fds, 2, -1);
			if (num_fds_ready == -1) {
        			perror("Poll error");
				break;
	    		}

			if (num_fds_ready > 0) {
				for (int i = 0; i < 2; ++i) {
			        	if (fds[i].revents & POLLIN) {
			        		int valread = read(fds[i].fd, msg_buffer, BUFFER_SIZE);
					        if (valread < 0) {
							perror("Read error");
							break;
						} else {
							printf("User %d: %s\n", i + 1, msg_buffer);
						}
					}
				}
			}

			memset(msg_buffer, 0, BUFFER_SIZE);
		}
	}


}
