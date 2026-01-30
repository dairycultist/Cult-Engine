.PHONY: all clean

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin) # macOS
	GCCFLAGS = -lGLEW -framework OpenGL
else ifeq ($(UNAME), Linux)
	GCCFLAGS = -lGLEW -lGL -lm
else
	$(error Unsupported OS: $(UNAME))
endif

# recipes
all: client.out server.out

client: client.out
	@./client.out 127.0.0.1 25565

server: server.out
	@./server.out 127.0.0.1 25565

client.out: client.c packet.c
	@gcc client.c -o client.out $(GCCFLAGS) $(shell pkg-config --cflags --libs sdl2 SDL2_image)

server.out: server.c packet.c
	@gcc server.c -o server.out

clean:
	@rm client.out
	@rm server.out