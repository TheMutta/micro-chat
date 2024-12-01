#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

size_t clients;
struct pollfd *client_fds;
char msg_buffer[BUFFER_SIZE];

void *handle_client(void *arg) {
	memset(msg_buffer, 0, BUFFER_SIZE);

	for (;;) {
		int num_fds_ready = poll(client_fds, clients, 0);
		if (num_fds_ready == -1) {
			perror("Poll error");
			break;
		}

		if (num_fds_ready > 0) {
			for (int i = 0; i < clients; ++i) {
				if (client_fds[i].revents & POLLIN) {
					int valread = read(client_fds[i].fd, msg_buffer, BUFFER_SIZE);
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

	return NULL;
}

int main(int argc, char **argv) {
	if (argc != 2) return -1;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
	}

	listen(sockfd, 5);

	clients = 0;
	pthread_t client_thread;
	struct sockaddr_in *client_addrs = calloc(sizeof(struct sockaddr_in), 1);
	client_fds = calloc(sizeof(struct pollfd), 1);
	socklen_t client_length = sizeof(*client_addrs);
	pthread_create(&client_thread, NULL, handle_client, NULL);

	for(;;) {
		int clientfd = accept(sockfd, (struct sockaddr *)&client_addrs[clients], &client_length);
			
		client_fds[clients].fd = clientfd;
		client_fds[clients].events = POLLIN;

		clients++;

		client_fds = realloc(client_fds, sizeof(struct pollfd) * (1 + clients));
		client_addrs = realloc(client_addrs, sizeof(struct sockaddr_in) * (1 + clients));

		client_fds[clients].fd = 0;
		client_fds[clients].events = 0;
		
		printf("Added client. Now at: %lu\n", clients);
	}


}
