print("starting client")

local f = assert(loadlib("client/context.so", "luaopen_contextlib"))
f()  -- actually open the library

contextlib.init(640, 320)

while true do
    contextlib.present()

    if contextlib.populate_input() then
        break
    end
end