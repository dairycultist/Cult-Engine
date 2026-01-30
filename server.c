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

#define LOGIC_INTERVAL_US (1000000 / 60)
#define MAX_PLAYER_COUNT 10
#define CONN_BACKLOG 4

typedef struct {

	int server_fd;
	struct pollfd client_fds[MAX_PLAYER_COUNT];

} NetworkInfo;

static void on_client_packet(NetworkInfo *network_info, Packet *packet, int client_fd) {

	switch (packet->packet_type) {

		case P_PING:
			printf("%d pinged: %s\n", client_fd, packet->big_body);
			packet->packet_type = P_PONG;
			write_packet(client_fd, packet);
			break;

		case P_PONG:
			printf("%d ponged: %s\n", client_fd, packet->big_body);
			break;
	}
}

static void on_tick(NetworkInfo *network_info) {
	// TODO client_fds should be accessible here
}

static void *handle_clients(void *network_info_void) {

	// mutexes are a thing
	// pthread_mutex_t, pthread_mutex_lock(), pthread_mutex_unlock()

	NetworkInfo *network_info = (NetworkInfo *) network_info_void;

	while (1) {

		// accept any new clients
		int client_fd = accept(network_info->server_fd, NULL, NULL);

		if (client_fd != -1) {
			
			// ensure client file descriptor is blocking (unlike server)
			fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL, 0) & ~O_NONBLOCK);

			// find a space to insert the client
			for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

				if (network_info->client_fds[i].fd == -1) {

					printf("%d connected.\n", client_fd);
					network_info->client_fds[i].fd = client_fd;
					goto login_success;
				}
			}

			// if there was no space, disconnect the client
			printf("%d tried to connect, but exceeded the player cap.\n", client_fd);
			close(client_fd);
		}

		login_success:

		// handle incoming packets
		if (poll(network_info->client_fds, MAX_PLAYER_COUNT, 0) != 0) {

			for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

				int client_fd = network_info->client_fds[i].fd;

				// packet received
				if (network_info->client_fds[i].revents & POLLIN) {

					static Packet packet;

					read_packet(client_fd, &packet);

					on_client_packet(network_info, &packet, client_fd);
				}

				// client socket was closed unexpectedly (i.e. no disconnect packet)
				if (network_info->client_fds[i].revents & (POLLERR | POLLHUP)) {

					printf("%d disconnected\n", client_fd);

					close(client_fd);
					network_info->client_fds[i].fd = -1;
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
	static NetworkInfo network_info;
	network_info.server_fd = socket(AF_INET, SOCK_STREAM, 0);

	// make socket non-blocking
	fcntl(network_info.server_fd, F_SETFL, fcntl(network_info.server_fd, F_GETFL, 0) | O_NONBLOCK);

	// initialize server information
	{
		struct sockaddr_in server_addr;

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(atoi(argv[2]));

		if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
			perror("Invalid address.");
			return errno;
		}

		if (bind(network_info.server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
			perror("Failed to bind.\n");
			return errno;
		}

		if (listen(network_info.server_fd, CONN_BACKLOG)) {
			perror("Failed to listen.\n");
			return errno;
		}
	}

	printf("Server initialized at %s:%s\n", argv[1], argv[2]);

	// initialize client file descriptor list
	for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

		network_info.client_fds[i].fd     = -1;
		network_info.client_fds[i].events = POLLIN;
	}

	// create the client handling thread
	pthread_t client_handler;

	if (pthread_create(&client_handler, NULL, handle_clients, &network_info)) {

		perror("Failed to create client handling thread.\n");
		return 1;
	}

	// main loop (performs client-independent logic)
	while (1) {

		on_tick(&network_info);
		usleep(LOGIC_INTERVAL_US);
	}

	// pthread_join(client_handler, NULL);
	// shutdown(server_fd, SHUT_RDWR);
	// close(server_fd);

	// return 0;
}