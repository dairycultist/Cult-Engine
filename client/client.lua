print("starting client")

-- open C libraries
assert(loadlib("client/contextlib.so", "luaopen_contextlib"))()

contextlib.init("Cult Engine")

-- networklib.send("set_position", ...)

local texture = contextlib.create_texture("./test.png")
local mesh = contextlib.create_mesh("./test.obj")

local r = 0.0

while true do
    r = r + 0.01
    contextlib.set_camera(0, 0, 0, 0, r)
    contextlib.draw_mesh(mesh, texture, 0, 0, -5, 0, 0)
    contextlib.present()
    if contextlib.populate_input() then
        break
    end
end