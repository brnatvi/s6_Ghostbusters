all: ghost_client

ghost_client: main.c utils.h utils.c common.h socket.c socket.h protocol.h protocol.c list.c log.h log.c engine.c engine.h
	gcc -I headers -o ghost_client main.c utils.c socket.c protocol.c list.c engine.c log.c

clean:
	rm -rf ghost_client *.o