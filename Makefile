.PHONY: all clean

all: client.out server.out

client: client.out
	@./client.out 127.0.0.1 25565

server: server.out
	@./server.out 127.0.0.1 25565

client.out: client.c packet.c
	@gcc client.c -o client.out

server.out: server.c packet.c
	@gcc server.c -o server.out

clean:
	@rm client.out
	@rm server.out