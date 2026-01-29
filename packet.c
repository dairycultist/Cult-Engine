// shared by both server.c and client.c for abstracting packet structure

// a packet is structured as such:
// 32 bits for the packet type
// 32 bits for the body size (can indicate up to over 4GB; is the size of the packet excluding the packet type and body size)
// an arbitrary amount of bytes for the body, whose structure depends on the packet type

// the small body exists for very common, small communications (like sending a player's position)
// that would needlessly consume resources were they to be constantly mallocing and freeing the big_body
#define SMALL_BODY_LEN 8

// StoC or CtoS: When recieved, the reciever should send the big body verbatim back to the sender without modification with the P_PONG packet type.
#define P_PING 0
#define P_PONG 1

void read_packet(int fd, uint32_t *packet_type, uint32_t *big_body_size, float small_body[SMALL_BODY_LEN], char **big_body) {

	// read metadata
	read(fd, packet_type, 4);
	read(fd, big_body_size, 4);

	// read body (we know the size of the small body from the packet type)
	*big_body = malloc(*big_body_size);
	read(fd, *big_body, *big_body_size);
}

void write_packet(int fd, uint32_t packet_type, uint32_t big_body_size, const float small_body[SMALL_BODY_LEN], const char *big_body) {
	
	// write metadata
	write(fd, &packet_type, sizeof(uint32_t));
	write(fd, &big_body_size, sizeof(uint32_t));

	// write body
	write(fd, big_body, big_body_size);
}