#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/*
 * context creation/rendering/input logic
 */

static SDL_Window *window;
static int running = 1;

#include "mesh.c"

static int init(lua_State *state) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
		return luaL_error(state, "Could not initialize SDL: %s\n", SDL_GetError());

	// init OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	window = SDL_CreateWindow(luaL_checkstring(state, 1), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	if (!window)
		return luaL_error(state, "Could not create window: %s\n", SDL_GetError());

	SDL_SetWindowMinimumSize(window, 600, 400);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glClearColor(1.0f, 0.188f, 0.647f, 1.0f); // since the sky is rendered as a mesh, set the clear color to hot pink so it's obvious
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	create_shader_program(&shader3D_program, vertex3D, fragment3D);

	return 0;
}

static int populate_input(lua_State *state) {

    SDL_Event event;

	int quit = 0;

    while (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT) {

            quit = 1;

        } else if (event.type == SDL_KEYDOWN && event.key.repeat == 0 && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {

			int prev_was_relative = SDL_GetRelativeMouseMode();
			SDL_SetRelativeMouseMode(!prev_was_relative);

			if (prev_was_relative) {

				int w, h;
				SDL_GetWindowSize(window, &w, &h);
				SDL_WarpMouseInWindow(window, w / 2, h / 2);
			}
		}
    }

	lua_pushboolean(state, quit);

	return 1;
}

static int present(lua_State *state) {

    SDL_GL_SwapWindow(window);
    SDL_Delay(1000 / 60);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return 0;
}

/*
 * register C functions as a Lua library
 */
static const struct luaL_reg contextlib[] = {

	// context
    {"init", init},
	{"populate_input", populate_input},
	{"present", present},

	// mesh
	{"create_texture", create_texture},
	{"create_mesh", create_mesh},
	// {"set_camera", set_camera},
	{"draw_mesh", draw_mesh},

    {NULL, NULL}
};

int luaopen_contextlib(lua_State *L) {
    luaL_openlib(L, "contextlib", contextlib, 0);
    return 1;
}