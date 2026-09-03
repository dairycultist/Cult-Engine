# Cult-Engine
A work-in-progress multiplayer sandbox game with server-side modding support in Lua.

All higher-order logic will be written in Lua. [Lua calls C](https://www.lua.org/pil/26.html) to perform lower-order operations like rendering, networking, and collision.

## Goals
- [ ] Multiplayer (two different codebases, one for the server and one for the client)
- [ ] 3D rendering from assets sent by the server
- [ ] Map system
- [ ] Item system

## Dependencies

### Lua 5.0.3

Download and unzip the Lua 5.0.3 source:

```
curl -L -R -O https://www.lua.org/ftp/lua-5.0.3.tar.gz
tar zxf lua-5.0.3.tar.gz
```

Open the created folder `lua-5.0.3` in a text editor and uncomment the following lines (remove the leading `#`):

```
#LOADLIB= -DUSE_DLOPEN=1
#DLLIB= -ldl

#MYLDFLAGS= -Wl,-E
```

Build Lua:

```
cd lua-5.0.3
make all test
```

Finally, add Lua to your CLI commands.

```
printf "\nalias lua='%s'\n" $(realpath bin/lua) >> ~/.bashrc
printf "\nalias luac='%s'\n" $(realpath bin/luac) >> ~/.bashrc
```

## Notes
Publish a playtest on itch.io to get feedback and motivation to continue working on it

like roblox or gmod

be not worrisome and perfectionistic

I like C and I like playing with my friends and I like modding

modding is solely server-side, but the server has a lot of control over the client, like hytale
