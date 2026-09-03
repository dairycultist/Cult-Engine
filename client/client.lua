print("starting client")

local f = assert(loadlib("client/render.so", "luaopen_mylib"))
f()  -- actually open the library

print(mylib.myfunc(20))