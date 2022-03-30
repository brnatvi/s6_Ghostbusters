all: server

server: server.c
	gcc -Wall -I headers -o server server.c

clean:
	rm -rf server *.o