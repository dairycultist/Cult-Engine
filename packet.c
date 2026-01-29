// shared by both server.c and client.c for abstracting packet structure

// a packet is structured as such:
// 32 bits for the packet type
// 32 bits for the body size (can indicate up to over 4GB; is the size of the packet excluding the packet type and body size)
// an arbitrary amount of bytes for the body, whose structure depends on the packet type

void read_packet(int fd, uint32_t *packet_type, uint32_t *big_body_size, float small_body[8], char **big_body) {

	// read metadata
	read(fd, packet_type, 4);
	read(fd, big_body_size, 4);

	// read body (we know the size of the small body from the packet type)
	*big_body = malloc(*big_body_size);
	read(fd, *big_body, *big_body_size);
}

void write_packet() {
	
}