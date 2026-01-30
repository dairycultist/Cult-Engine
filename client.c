#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "packet.c"

typedef struct {

	// contains socket, information about the gamestate (like entities and objs), and whether or not the session has ended

} SessionInfo;

int main(int argc, char **argv) {

	// create server connection
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

		if (connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
			perror("Connection failed.");
			return errno;
		}
	}

	// create two threads; one for handling server messages and one for server-independent logic (including rendering!)

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



int server_independent() {

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Could not initialize SDL (%s)\n", SDL_GetError());
		return 1;
	}

	// init OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// create the window
	SDL_Window *window = SDL_CreateWindow("Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window) {
		fprintf(stderr, "Could not create window (%s)\n", SDL_GetError());
		return 1;
    }

	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!context) {
		fprintf(stderr, "Could not create context (%s)\n", SDL_GetError());
		return 1;
    }

	glewExperimental = GL_TRUE;
	glewInit();

	// enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// enable backface culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	// initialize rendering
	// initialize_shaders();
	// initialize_perspective(2.0);

	// process events until window is closed
	SDL_Event event;
	int running = 1;

	while (running) {

		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {

				running = 0;

				// TODO send disconnect packet

			} else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {

				glViewport(0, 0, event.window.data1, event.window.data2);
				// initialize_perspective(event.window.data1 / (float) event.window.data2);
			
			} else {

				// input handling
			}
		}

		// client-side (server-independent) logic

		SDL_GL_SwapWindow(window);
		SDL_Delay(1000 / 60);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// free everything
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}