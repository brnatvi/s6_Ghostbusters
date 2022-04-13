all: server

server: src/server.c src/server_aux_functions.c src/list.c
	gcc -pthread -Wall -I headers -o server src/server.c src/server_aux_functions.c src/list.c

distclean:
	rm -rf server *.o
clean:
	rm -rf server *.o