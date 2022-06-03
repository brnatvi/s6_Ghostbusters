#include "server.h"
#include "commons.h"
#include <signal.h>

int main(int argc, char **argv)
{
    SET_TERMINAL_COLOR(DEFAULT_TERM_COLOR);

    sigaction(SIGPIPE, &(struct sigaction){{SIG_IGN}}, NULL);
    srand(time(NULL)); 
    int rez = EXIT_SUCCESS;

    // declaration & init
    if ((argc != 3) || (0 == strcmp(argv[1], "--help")))
    {
        printf("> Run :\n> ./server <port> <max size>\n");
        printf("> N.B. <max size> has to be 0 or > 10\n");
        printf("> if <max size> = 0 then maze size will be randomly selected from 10 to 999\n");
        printf(">                   else maze size will be randomly selected from 10 to <max size>\n");
        return rez;
    }

    int port  = atoi(argv[1]);
    int maxSz = atoi(argv[2]);
    if ((maxSz < 0) || ((maxSz >= 1) && (maxSz < MIN_MAZE_SIZE)))
    {
        log_error("Wrong input, <max size> is out of range, accepted value 0 or > 10");
        return EXIT_FAILURE;
    }
    
    int tcpListenSocket = -1;

    // create context for connection
    struct stServerContext *context = (struct stServerContext *)malloc(sizeof(struct stServerContext));
    if (!context)
    {
        log_fatal("malloc stServerContext");
        goto lExit;
    }
    memset(context, 0, sizeof(struct sockaddr_in));
    context->tcpListenSocket = tcpListenSocket;

    struct sockaddr_in *socAddress = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    if (!socAddress)
    {
        log_fatal("malloc socAddress");
        goto lExit;
    }
    memset(socAddress, 0, sizeof(struct sockaddr_in));
    context->mazeSzLimite = (uint16_t)maxSz;
    context->sockAddress = *socAddress;
    context->lastGameId = 0;

    // fill sockaddr_in
    context->sockAddress.sin_family      = AF_INET;
    context->sockAddress.sin_port        = htons(port);              
    context->sockAddress.sin_addr.s_addr = htonl(INADDR_ANY); 

    pthread_mutexattr_t Attr;
    pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&context->serverLock, &Attr);

    context->lstGames = (struct listElements_t *)malloc(sizeof(struct listElements_t));
    if (!context->lstGames)
    {
        log_fatal("malloc context->lstGames");
        goto lExit;
    }
    memset(context->lstGames, 0, sizeof(struct listElements_t));

    // create TCP socket (bind port) and wait for connection
    int r = createTcpConnection(context, port);
    if (r < 0)
    {
        rez = EXIT_FAILURE;
        goto lExit;
    }

    while (1)
    {
        // accept and routine_com before the game
        int rezAccept = acceptAndCommunication(context);
        if (EXIT_FAILURE == rezAccept) // TODO if need to crush the server if one accept is failure ??
        {
            printf("ACCEPT FAILURE!\n");
            rez = EXIT_FAILURE;
            goto lExit;
        }
    }

lExit:
    FREE_MEM(socAddress);    

    while (context->lstGames->first)
    {
        struct stGame *game = (struct stGame *)(context->lstGames->first->data);      
        freeGame(game);
        removeEl(context->lstGames, context->lstGames->first);
    }
    FREE_MEM(context->lstGames);

    FREE_MEM(context);
    CLOSE(tcpListenSocket);
    return rez;
}