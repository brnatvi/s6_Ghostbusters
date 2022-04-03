#include "server.h"
#include "commons.h"


int main(int argc, char **argv)
{
    // declaration & init   
    int port = 4242;
    int rez = EXIT_SUCCESS;
    int fd1 = -1;
    
    // create context for connection
    struct stConnection* connect = (struct stConnection *) malloc(sizeof(struct stConnection));
    if (!connect)
    {
        perror("malloc stConnection");
        goto lExit;
    }
    memset(connect, 0, sizeof(struct sockaddr_in));
    connect->fd1 = fd1;
    
    struct sockaddr_in* socAddress = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    memset(socAddress, 0, sizeof(struct sockaddr_in));
    connect->sockAddress = *socAddress;
    // fill sockaddr_in
    connect->sockAddress.sin_family = AF_INET;
    connect->sockAddress.sin_port = htons(port);                       // TODO check that need htons
    connect->sockAddress.sin_addr.s_addr = htonl(INADDR_ANY);          // TODO check that need htons

    connect->games = (struct listElements_t*) malloc(sizeof(struct listElements_t));           // TODO BAD!!!
    if (!connect->games)
    {
        perror("malloc listElements_t");
        goto lExit;
    }
    connect->games->first = NULL;
  
     
    // create TCP socket (bind port) and wait for connection 
    int r = createTcpConnection(connect, port);
    if (r < 0)
    {
        rez = EXIT_FAILURE;
        goto lExit;
    }
    printf("connection created : \nfd1 = %d\nsin_addr = %d\nport = %d\n", connect->fd1, connect->sockAddress.sin_addr.s_addr, connect->sockAddress.sin_port);
        

    while (1)
    {
        // accept and communication before the game
        int rezAccept = acceptAndCommunication(connect);
        if (EXIT_FAILURE == rezAccept)                        // TODO if need to crush the server if one accept is failure ??
        {
            rez = EXIT_FAILURE;
            goto lExit;
        }       
        
    }
    



lExit:
    FREE_MEM(socAddress);
    FREE_MEM(connect);
    CLOSE(fd1);
    return rez;
}