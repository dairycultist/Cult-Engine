#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>

#include <arpa/inet.h>

#define MAX_PLAYER_COUNT 10
#define CONN_BACKLOG 4

#define IP "127.0.0.1"
#define PORT 25565

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

					client_fds[i].fd = client_fd;
					goto login_success;
				}
			}

			// if there was no space, disconnect the client
			close(client_fd);
		}

		login_success:

		// handle incoming packets
		if (poll(client_fds, MAX_PLAYER_COUNT, 0) != 0) {

			for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

				if (client_fds[i].revents & POLLIN) {

					// packet received
					char in;
					read(client_fds[i].fd, &in, 1);
					printf("%c", in);
				}

				if (client_fds[i].revents & (POLLERR | POLLHUP)) {

					// client socket was suddenly closed (no disconnect packet)
					printf("\n");

					close(client_fds[i].fd);
					client_fds[i].fd = -1;
				}
			}
		}
	}

	return NULL;
}

int main() {

	// create server socket
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	// make socket non-blocking
	fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);

	// initialize server information
	{
		struct sockaddr_in server_addr;

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(PORT);
		server_addr.sin_addr.s_addr = inet_addr(IP);

		if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
			printf("Failed to bind.\n");
			return errno;
		}

		if (listen(server_fd, CONN_BACKLOG)) {
			printf("Failed to listen.\n");
			return errno;
		}
	}

	printf("Server initialized.\n");

	// create the client handling thread
	pthread_t client_handler;

	if (pthread_create(&client_handler, NULL, handle_clients, &server_fd)) {

		printf("Failed to create client handling thread.\n");
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