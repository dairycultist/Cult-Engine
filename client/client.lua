print("starting client")

-- open C libraries
assert(loadlib("client/contextlib.so", "luaopen_contextlib"))()

contextlib.init("Cult Engine")

-- networklib.send("set_position", ...)

while true do
    contextlib.present()

    if contextlib.populate_input() then
        break
    end
end