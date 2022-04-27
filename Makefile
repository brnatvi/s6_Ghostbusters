all: server test

server: src/server.c src/server_aux_functions.c src/list.c
	gcc -D _IS_SERVER_ -pthread -Wall -I headers -o server src/server.c src/server_aux_functions.c src/list.c

test: src/test.c src/server_aux_functions.c src/list.c
	gcc -D _IS_CLIENT_ -pthread -Wall -I headers -o test src/test.c src/server_aux_functions.c src/list.c

distclean:
	rm -rf server test *.o
clean:
	rm -rf server test *.o