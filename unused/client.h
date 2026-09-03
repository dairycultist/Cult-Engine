#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <stdint.h>

#include <arpa/inet.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct {

	int fd; // socket
	int BOOL_active; // whether or not the session has ended

	// will also contain information about the gamestate (like entities and objs)

} SessionInfo;

void *handle_server(void *session_info_void);
void on_tick(SessionInfo *session_info);

#endif