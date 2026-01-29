#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <stdint.h>

#include <arpa/inet.h>

#include "packet.c"

#define MAX_PLAYER_COUNT 10
#define CONN_BACKLOG 4

static void *handle_clients(void *server_fd) {

	// mutexes are a thing
	// pthread_mutex_t, pthread_mutex_lock(), pthread_mutex_unlock()

	struct pollfd client_fds[MAX_PLAYER_COUNT];

	// initialize file descriptor list
	for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

		client_fds[i].fd     = -1;
		client_fds[i].events = POLLIN;
	}

	while (1) {

		// accept any new clients
		int client_fd = accept(*(int *) server_fd, NULL, NULL);

		if (client_fd != -1) {
			
			// ensure client file descriptor is blocking (unlike server)
			fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL, 0) & ~O_NONBLOCK);

			// find a space to insert the client
			for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

				if (client_fds[i].fd == -1) {

					printf("%d connected.", client_fd);
					client_fds[i].fd = client_fd;
					goto login_success;
				}
			}

			// if there was no space, disconnect the client
			printf("%d tried to connect, but exceeded the player cap.", client_fd);
			close(client_fd);
		}

		login_success:

		// handle incoming packets
		if (poll(client_fds, MAX_PLAYER_COUNT, 0) != 0) {

			for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

				if (client_fds[i].revents & POLLIN) {

					// packet received
					static uint32_t packet_type;
					static uint32_t big_body_size;
					static uint32_t small_body[SMALL_BODY_LEN];
					static char *big_body;

					read_packet(client_fds[i].fd, &packet_type, &big_body_size, small_body, &big_body);

					switch (packet_type) {

						case P_PING:
							printf("%d pinged: %s\n", client_fds[i].fd, big_body);
							write_packet(client_fds[i].fd, P_PONG, big_body_size, NULL, big_body);
							break;

						case P_PONG:
							printf("%d ponged: %s\n", client_fds[i].fd, big_body);
							break;
					}
				}

				if (client_fds[i].revents & (POLLERR | POLLHUP)) {

					// client socket was suddenly closed (no disconnect packet)
					printf("%d disconnected\n", client_fds[i].fd);

					close(client_fds[i].fd);
					client_fds[i].fd = -1;
				}
			}
		}
	}

	return NULL;
}

int main(int argc, char **argv) {

	if (argc != 3) {
		printf("Format: %s <IP> <PORT>\n", argv[0]);
		return 0;
	}

	// create server socket
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	// make socket non-blocking
	fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);

	// initialize server information
	{
		struct sockaddr_in server_addr;

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(atoi(argv[2]));

		if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
			perror("Invalid address.");
			return errno;
		}

		if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
			perror("Failed to bind.\n");
			return errno;
		}

		if (listen(server_fd, CONN_BACKLOG)) {
			perror("Failed to listen.\n");
			return errno;
		}
	}

	printf("Server initialized.\n");

	// create the client handling thread
	pthread_t client_handler;

	if (pthread_create(&client_handler, NULL, handle_clients, &server_fd)) {

		perror("Failed to create client handling thread.\n");
		return 1;
	}

	// main loop
	while (1) {

		// process non-client stuff
	}

	// pthread_join(client_handler, NULL);
	// shutdown(server_fd, SHUT_RDWR);
	// close(server_fd);

	// return 0;
}