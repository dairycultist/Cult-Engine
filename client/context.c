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

static int init(lua_State *state) {

	// double d = luaL_checknumber(state, 1); // throws a Lua error for us if not a number

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// init OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	window = SDL_CreateWindow("Cult Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 200, SDL_WINDOW_OPENGL);

	if (!window) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		exit(1);
    }

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glClearColor(1.0f, 0.188f, 0.647f, 1.0f); // since the sky is rendered as a mesh, set the clear color to hot pink so it's obvious
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// SDL_SetRelativeMouseMode(SDL_TRUE);

	return 0;
}

static int populate_input(lua_State *state) {

    SDL_Event event;

	int quit = 0;

    while (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT) {

            quit = 1;
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
    {"init", init},
	{"populate_input", populate_input},
	{"present", present},
    {NULL, NULL}
};

int luaopen_contextlib(lua_State *L) {

    luaL_openlib(L, "contextlib", contextlib, 0);
    return 1;
}