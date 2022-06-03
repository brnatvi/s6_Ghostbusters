# Ghostbusters
A multiplayer game based on client-server architecture.
The goal of the game is to catch as many ghosts as possible while moving through the labyrinth. Configuration of the maze is not visible. Information about it, as well as about the location of ghosts, is obtained in form of requests information from server.

## Owners
BRAGINA Natalia and VIGNESWARAN Tharsiya

Parts of project:
- server on C (/serveur)
- client on Java with GUI (/client)
- client on C with CLI (/c_client)

## Compilation :
Parts on C language
```sh
make
```

Parts on Java
```sh
javac *.java
```

## Run :
```sh
./server <port> <max size>
N.B. <max size> = 0 or >10
     if <max size> = 0 then the maze will be created with random dimentions from 10 to 999
                       else the maze will be created with random dimentions from 10 to <max size>
```

```sh
java Client <server's hostname> <port>
```
N.B. <port> must be 4 characters long.





Multicast address used is 239.255.255.250

## «Cheat-mode» - automatic execution
The CLI-client makes possible to launch the program in "Cheat-mode" - automatic execution of traversal of the labyrinth.
To use it please move to directory «c_client», compile then run.

Run : 
```sh
./ghost_client <server's hostname> <player id> <port>
```
N.B. <player id> must be 8 characters long, <port> - 4 characters long.

Then follow the text menus :
- 3 -> Create new game
- 2 -> Start game
- 7 -> On/off cheat mode

## Logging
The client-server message exchange is logged. The red color transmits messages from the server, the green one from the client.
Errors etc are displayed according to level "trace", "debug", "info", "warn", "error", "fatal" (log.c library)

## External libraries usage
In our project we use the following external libraries :
- Logger : https://github.com/rxi/log.c 
- Maze generator
article : https://habr.com/ru/post/262345/
code : https://github.com/mersinvald/maze-generator_offscreen    

