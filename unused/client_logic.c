#include "client.h"

#include "packet.c"

void *handle_server(void *session_info_void) {

	SessionInfo *session_info = (SessionInfo *) session_info_void;

	while (session_info->BOOL_active) {
		usleep(1000000);
	}

	// // test transmission
	// static Packet packet;

	// packet.packet_type = P_PING;
	// packet.big_body_size = strlen("Hello there, server!") + 1;
	// packet.big_body = "Hello there, server!";

	// write_packet(fd, &packet);

	// read_packet(fd, &packet);

	// printf("Heard pong: %s\n", packet.big_body);

	return NULL;
}

void on_tick(SessionInfo *session_info) {

}