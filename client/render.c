#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"

// gcc -shared -o client/render.so -fPIC client/render.c

static int l_myfunc(lua_State *state) {
    double d = luaL_checknumber(state, 1); // throws a Lua error for us if not a number
    lua_pushnumber(state, d * 10.0);
    return 1;  // number of results
}

// register C functions so that they can later be called by Lua code
static const struct luaL_reg mylib[] = {
    {"myfunc", l_myfunc},
    {NULL, NULL} // terminator
};

int luaopen_mylib(lua_State *L) {
    luaL_openlib(L, "mylib", mylib, 0);
    return 1;
}