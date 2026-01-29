// shared by both server.c and client.c for abstracting packet structure

// a packet is structured as such:
// 32 bits for the packet type
// 32 bits for the body size (can indicate up to over 4GB; is the size of the packet excluding the packet type and body size)
// an arbitrary amount of bytes for the body, whose structure depends on the packet type

// the small body exists for very common, small communications (like sending a player's position)
// that would needlessly consume resources were they to be constantly mallocing and freeing the big_body.
// (position will have to be quantized, whatever; entity and obj IDs will also be sent in the small body)
#define SMALL_BODY_LEN 8

typedef struct {

	uint32_t packet_type;
	uint32_t big_body_size;
	uint32_t small_body[SMALL_BODY_LEN];
	char *big_body;

} Packet;

// StoC or CtoS: When recieved, the reciever should send the big body verbatim back to the sender without modification with the P_PONG packet type.
#define P_PING 0
#define P_PONG 1
// // StoC: Sends an object file's contents verbatim to the client.
// #define P_OBJ 2
// // CtoS
// #define P_CLIENT_POS_ROT 3
// // StoC: Updates the position and rotation of an entity (including other players) on the client-side. Format: id, x, y, z, pitch, yaw
// #define P_ENTITY_POS_ROT 4

// probably also something to send colliders; player input, movement, and collision is performed on the client-side since idc about hacking

void read_packet(int fd, Packet *packet) {

	// read metadata
	read(fd, &packet->packet_type, sizeof(uint32_t));
	read(fd, &packet->big_body_size, sizeof(uint32_t));

	// read body (we know the size of the small body from the packet type)
	packet->big_body = malloc(packet->big_body_size);
	read(fd, packet->big_body, packet->big_body_size);
}

void write_packet(int fd, const Packet *packet) {
	
	// write metadata
	write(fd, &packet->packet_type, sizeof(uint32_t));
	write(fd, &packet->big_body_size, sizeof(uint32_t));

	// write body
	write(fd, packet->big_body, packet->big_body_size);
}