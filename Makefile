all: server client

server: src/server.c src/server_aux_functions.c src/list.c
	gcc -pthread -Wall -I headers -o server src/server.c src/server_aux_functions.c src/list.c

client: src/client.c src/server_aux_functions.c src/list.c
	gcc -pthread -Wall -I headers -o client src/client.c src/server_aux_functions.c src/list.c

distclean:
	rm -rf server client *.o
clean:
	rm -rf server client *.o