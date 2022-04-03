all: server

server: src/server.c
	gcc -pthread -Wall -I headers -o server src/server.c src/server_aux_functions.c src/list.c

clean:
	rm -rf server *.o