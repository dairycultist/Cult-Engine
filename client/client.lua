print("starting client")

local f = assert(loadlib("client/context.so", "luaopen_contextlib"))
f()  -- actually open the library

contextlib.init()

while true do
    contextlib.present()

    if contextlib.populate_input() then
        break
    end
end