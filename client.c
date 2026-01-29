#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "packet.c"

int main(int argc, char **argv) {

	if (argc != 3) {
		printf("Format: %s <IP> <PORT>\n", argv[0]);
		return 0;
	}
	
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        perror("socket creation failed");
        return errno;
    }

	{
		struct sockaddr_in serv_addr;

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(atoi(argv[2]));

		if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
			perror("Invalid address.");
			return errno;
		}

		if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
			perror("Connection failed.");
			return errno;
		}
	}

	// test transmission
	static Packet packet;

	packet.packet_type = P_PING;
	packet.big_body_size = strlen("Hello there, server!") + 1;
	packet.big_body = "Hello there, server!";

	write_packet(fd, &packet);

	read_packet(fd, &packet);

	printf("Heard pong: %s\n", packet.big_body);

    // close the socket
    close(fd);

    return 0;
}
