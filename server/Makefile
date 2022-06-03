all: server

srv_src = src/server.c\
          src/server_aux_functions.c\
		  src/maze_generator.c\
		  src/list.c\
		  src/protocol.c\
		  src/engine.c\
		  src/log.c

srv_hdr = headers/server.h\
          headers/server_aux_functions.h\
		  headers/maze_generator.h\
		  headers/list.h\
		  headers/protocol.h\
		  headers/commons.h\
		  headers/engine.h\
		  headers/log.h

server: $(srv_src) $(srv_hdr)
	gcc -D _IS_SERVER_ -pthread -Wall -I headers -o server $(srv_src)



distclean:
	rm -rf server test *.o
clean:
	rm -rf server test *.o
clear:
	rm -rf server test *.o