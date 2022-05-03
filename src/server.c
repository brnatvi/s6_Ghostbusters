#include "server.h"
#include "commons.h"
#include <signal.h>

int main(int argc, char **argv)
{
    sigaction(SIGPIPE, &(struct sigaction){{SIG_IGN}}, NULL);
    srand(time(NULL)); 

    // declaration & init
    int port = 4242;
    int rez = EXIT_SUCCESS;
    int tcpListenSocket = -1;

    // create context for connection
    struct stServerContext *context = (struct stServerContext *)malloc(sizeof(struct stServerContext));
    if (!context)
    {
        perror("malloc stServerContext");
        goto lExit;
    }
    memset(context, 0, sizeof(struct sockaddr_in));
    context->tcpListenSocket = tcpListenSocket;

    struct sockaddr_in *socAddress = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    memset(socAddress, 0, sizeof(struct sockaddr_in));
    context->sockAddress = *socAddress;
    context->lastGameId = 0;

    // fill sockaddr_in
    context->sockAddress.sin_family = AF_INET;
    context->sockAddress.sin_port = htons(port);              // TODO check that need htons
    context->sockAddress.sin_addr.s_addr = htonl(INADDR_ANY); // TODO check that need htons

    pthread_mutexattr_t Attr;
    pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&context->serverLock, &Attr);

    context->lstGames = (struct listElements_t *)malloc(sizeof(struct listElements_t));
    if (!context->lstGames)
    {
        perror("malloc context->lstGames");
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
    // printf("connection created : \nfd1 = %d\nsin_addr = %d\nport = %d\n", context->tcpListenSocket, context->sockAddress.sin_addr.s_addr, context->sockAddress.sin_port);

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