#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "packet.c"

#define IP "127.0.0.1"
#define PORT 25565

int main() {
	
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        perror("socket creation failed");
        return errno;
    }

	{
		struct sockaddr_in serv_addr;

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(PORT);

		if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) {
			perror("Invalid address.");
			return errno;
		}

		if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
			perror("Connection failed.");
			return errno;
		}
	}

	// test transmission
	static uint32_t packet_type;
	static uint32_t big_body_size;
	static uint32_t small_body[SMALL_BODY_LEN];
	static char *big_body;

	write_packet(fd, P_PING, strlen("Hello there, server!") + 1, NULL, "Hello there, server!");

	read_packet(fd, &packet_type, &big_body_size, small_body, &big_body);

	printf("Heard pong: %s\n", big_body);

    // close the socket
    close(fd);

    return 0;
}
