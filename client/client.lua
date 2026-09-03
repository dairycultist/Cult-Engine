print("starting client")

-- open C libraries
assert(loadlib("client/contextlib.so", "luaopen_contextlib"))()

contextlib.init("Cult Engine")

-- networklib.send("set_position", ...)

local texture = contextlib.create_texture("./test.png")
local mesh = contextlib.create_mesh("./test.obj")

local camera_yaw, camera_pitch = 0, 0

while true do

    local dx, dy, exit = contextlib.populate_input()

    camera_yaw   = camera_yaw + dx * 0.01
    camera_pitch = math.min(math.max(camera_pitch + dy * 0.01, -1.57), 1.57)

    contextlib.set_camera(0, 0, 0, camera_pitch, camera_yaw)
    contextlib.draw_mesh(mesh, texture, 0, 0, -5, 0, 0)
    contextlib.present()

    if exit then
        break
    end
end