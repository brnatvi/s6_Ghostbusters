#include "server_aux_functions.h"
#include <fcntl.h>

#define PRINT_PROTOCOL

// create socket, bind, listen
// return 0 (success) or -1 (failure)
int createTcpConnection(struct stServerContext *context, int port)
{
    int rez = EXIT_SUCCESS;

    // fill sockaddr_in
    context->sockAddress.sin_family = AF_INET;
    context->sockAddress.sin_port = htons(port);              // TODO check that need htons
    context->sockAddress.sin_addr.s_addr = htonl(INADDR_ANY); // TODO check that need htons

    //create a socket
    context->fd1 = socket(AF_INET, SOCK_STREAM, 0);
    if (context->fd1 < 0)
    {
        perror("P: cannot create socket");
        rez = -1;
        goto lExit;
    }

    rez = bind(context->fd1, (struct sockaddr *)&context->sockAddress, sizeof(struct sockaddr_in));
    if (rez < 0)
    {
        perror("P: bind failure");
        goto lExit;
    }

    rez = listen(context->fd1, 0);
    if (rez < 0)
    {
        perror("P:listen failure");
        goto lExit;
    }

lExit:
    return rez;
}

// accept connection
// return 0 (success) or 1 (failure)
int acceptAndCommunication(struct stServerContext *context)
{
    int rez = EXIT_SUCCESS;
    pthread_t th;

    int fd2 = -1;
    socklen_t len = sizeof(context->sockAddress);

    // make accept
    fd2 = accept(context->fd1, (struct sockaddr *)&context->sockAddress, &len);
    if (fd2 < 0) // error
    {
        perror("P: accept failure");
        rez = EXIT_FAILURE;
    }
    else // session started
    {
        // create gamer and gamer's context
        struct stGamerContext *pGamerContext = (struct stGamerContext *)malloc(sizeof(struct stGamerContext));
        if (!pGamerContext)
        {
            perror("malloc gamer context failure");
            rez = EXIT_FAILURE;
            goto lExit;
        }

        pGamerContext->pServerCtx = context;
        pGamerContext->pGamer = createGamer(fd2, atoi(inet_ntoa(context->sockAddress.sin_addr))); // TODO check that need htons

        if (!pGamerContext->pGamer)
        {
            perror("malloc gamer failure");
            rez = EXIT_FAILURE;
            goto lExit;
        }

        // create thread of gamer for communication
        int phRez = pthread_create(&th, NULL, communication, (void *)pGamerContext);
        if (phRez != 0) // error
        {
            perror("P: pthread_create failure");
            rez = EXIT_FAILURE;
            goto lExit;
        }
    }

lExit:
    return rez;
}

// communication with client
void *communication(void *args)
{
    // // declaration & initialisation
    struct stGamerContext *pGamerContext = (struct stGamerContext *)(args);
    //struct stGamer gamer = context->gamer;
    int fd2 = pGamerContext->pGamer->fd2;

    ssize_t rezRecv = -1;
    ssize_t rezSend = -1;

    //char pseudo[SIZE_ID];
    char bufer[BUF_SIZE];
    char answer[BUF_SIZE];

    processINFO(pGamerContext, rezSend, answer);

    printGamers(pGamerContext, __FUNCTION__);

    // listen answers
    while (1)
    {
        rezRecv = recieveMessage(fd2, bufer, TCP_END);
        if (rezRecv < 0) //error occured
        {
            perror("recieve client's request failure");
            goto lClose;
        }
        else if (0 == rezRecv) // client has disconnected
        {
            goto lClose;
        }

        char req[20];
        strncpy(req, bufer, LEN_KEYWORD);
        req[LEN_KEYWORD] = 0;

        if (0 == strcmp(req, NEWPL))
        {
            processNEWPL(pGamerContext, bufer + LEN_KEYWORD + 1, rezSend, answer);
            printGamers(pGamerContext, "+processNEWPL");
        }
        else if (0 == strcmp(req, REGIS))
        {
            processREGIS(pGamerContext, bufer + LEN_KEYWORD + 1, rezSend, answer);
            printGamers(pGamerContext, "+processREGIS");
        }
        else if (0 == strcmp(req, UNREG))
        {
            processUNREG(pGamerContext, bufer + LEN_KEYWORD + 1, rezSend, answer);
            printGamers(pGamerContext, "+processUNREG");
        }
        // else if (0 == strcmp(req, SIZE_Q))
        // {
        //     processSIZE_(context, bufer, rezSend, answer);
        // }
        else if (0 == strcmp(req, LIST_Q))
        {
            printGamers(pGamerContext, "-processLIST_");
            processLIST_(pGamerContext, bufer + LEN_KEYWORD + 1, rezSend, answer);
        }
        else if (0 == strcmp(req, GAME_Q))
        {
            processGAME_(pGamerContext, rezSend, answer);
            printGamers(pGamerContext, "+processGAME_");
        }

        // else if (0 == strcmp(req, GAMES))
        // {
        //     processGAMES(context, bufer, rezSend, answer);
        // }

        //        - if START recieved from all gamers :
        //                      + create thread
        //                      + create UDP
        //                      + launch engine on it with this stGame
    }

lClose:
    //freeGamer(pGamerContext->pGamer);
    //freeGame(pGamerContext->pServerCtx->games);   // TODO Do not forget free all games at the end
    return NULL;
}

//////////////////////////// Process functions //////////////////////////////
////           return -1 if error, else number of sent bytes      ////////

// send info about all games and about each game
ssize_t processINFO(struct stGamerContext *gContext, ssize_t rezSend, char *answer)
{
    int fd2 = gContext->pGamer->fd2;

    pthread_mutex_lock(&gContext->pServerCtx->serverLock);
    uint8_t n = gContext->pServerCtx->games->count;
    pthread_mutex_unlock(&gContext->pServerCtx->serverLock);

    //send info about all games
    char *iter = answer;

    memcpy(iter, GAMES, strlen(GAMES));
    iter += strlen(GAMES);
    memcpy(iter, WHITE, strlen(WHITE));
    iter += strlen(WHITE);
    memcpy(iter, &n, sizeof(n));
    iter += sizeof(n);
    memcpy(iter, TCP_END, strlen(TCP_END));
    iter += strlen(TCP_END);

    ssize_t answerLen = (iter - answer);

    rezSend = send(fd2, answer, answerLen, 0);
    if (rezSend != answerLen)
    {
        perror("GAMES<n>*** sending failure");
        return -1;
    }

    //send info about each game
    struct element_t *gameEl = gContext->pServerCtx->games->first;
    struct stGame *game = NULL;
    while (gameEl)
    {
        game = (struct stGame *)(gameEl->data);

        char *iter = answer;

        memcpy(iter, OGAME, strlen(OGAME));
        iter += strlen(OGAME);
        memcpy(iter, WHITE, strlen(WHITE));
        iter += strlen(WHITE);
        memcpy(iter, &game->idGame, sizeof(game->idGame));
        iter += sizeof(game->idGame);
        memcpy(iter, WHITE, strlen(WHITE));
        iter += strlen(WHITE);
        memcpy(iter, &game->gamers->count, sizeof(game->gamers->count));
        iter += sizeof(game->gamers->count);
        memcpy(iter, TCP_END, strlen(TCP_END));
        iter += strlen(TCP_END);

        ssize_t answerLen = iter - answer;

        //#if defined(PRINT_PROTOCOL)
        //        iter = answer;
        //        for (ssize_t i = 0; i < answerLen; i++)
        //        {
        //            printf("0x%02X(%c) ", *iter, *iter ? *iter : '0');
        //            iter++;
        //        }
        //        printf("\n");
        //#endif

        rezSend = send(fd2, answer, answerLen, 0);

        if (rezSend != answerLen)
        {
            perror("OGAME<m><s>*** for sending failure");
            return -1;
        }
        gameEl = gameEl->next;
    }
    return rezSend;
}

//-> NEWPL id port*** -> ask create game
ssize_t processNEWPL(struct stGamerContext *gContext, char *bufer, ssize_t rezSend, char *answer)
{
    int fd2 = gContext->pGamer->fd2;
    char *iter = (char *)bufer;

    // create new game
    struct stGame *newGame = createGame(gContext, bufer);
    if (newGame) //REGOK m*** -> inscription OK
    {
        // add this game to list of games
        addGame(gContext, newGame); //ADD LAST

        // add name for this gamer
        strncpy(gContext->pGamer->id, iter, LEN_ID);
        iter += LEN_ID + 1;

        printf("from NEWPL 0 {%s}\n", gContext->pGamer->id);

        char port[20];
        strncpy(port, iter, LEN_PORT);
        port[LEN_PORT] = 0;
        gContext->pGamer->portUDP = atoi(port);

        // add this gamer to list of gamers
        addGamer(gContext, newGame, gContext->pGamer); //ADD LAST

        struct stGame *pGame = (struct stGame *)gContext->pServerCtx->games->last->data;
        struct stGamer *pGameR = (struct stGamer *)pGame->gamers->last->data;

        printf("from NEWPL 1 {%s}\n", pGameR->id);

        //compose answer
        iter = answer;
        memcpy(iter, REGOK, strlen(REGOK));
        iter += strlen(REGOK);
        memcpy(iter, WHITE, strlen(WHITE));
        iter += strlen(WHITE);
        memcpy(iter, &newGame->idGame, sizeof(newGame->idGame));
        iter += sizeof(newGame->idGame);
        memcpy(iter, TCP_END, strlen(TCP_END));
        iter += strlen(TCP_END);

        ssize_t answerLen = iter - answer;

        rezSend = send(fd2, answer, answerLen, 0);
        if (rezSend < answerLen)
        {
            perror("REGOK / REGNO sending failure");
            return -1;
        }
    }
    else // REGNO***
    {
        //compose answer
        sprintf(answer, "%s%s%c", REGNO, TCP_END, '\0');
        rezSend = send(fd2, answer, strlen(answer), 0);
        if (rezSend < strlen(answer))
        {
            perror("REGOK / REGNO sending failure");
            return -1;
        }
    }

    return rezSend;
}

//-> REGIS id port m*** -> subscribe to game m
ssize_t processREGIS(struct stGamerContext *gContext, char *bufer, ssize_t rezSend, char *answer)
{
    int fd2 = gContext->pGamer->fd2;
    char *iter = (char *)bufer;

    strncpy(gContext->pGamer->id, iter, LEN_ID);
    iter += LEN_ID + 1;

    char stPort[20];
    strncpy(stPort, iter, LEN_PORT);
    stPort[LEN_PORT] = 0;
    uint16_t port = atoi(stPort);
    gContext->pGamer->portUDP = port;
    // printf("%d\n", context->gamer->portUDP);
    iter += LEN_PORT + 1;

    uint8_t m = *(uint8_t *)iter;

    printf("{%s} asked game : %d\n", __FUNCTION__, m);

    // add gamer into list of gamers of this game
    struct element_t *gameEl = gContext->pServerCtx->games->first;
    unsigned int isFound = 0;
    while (gameEl)
    {
        struct stGame *game = (struct stGame *)(gameEl->data);

        // REGOK m*** -> inscription OK
        if (game->idGame == m)
        {
            addGamer(gContext, game, gContext->pGamer);

            struct stGamer *g = (struct stGamer *)game->gamers->last->data;

            printf("{%s} User ID{%s}\n", __FUNCTION__, g->id);

            char *iter = answer;
            memcpy(iter, REGOK, strlen(REGOK));
            iter += strlen(REGOK);
            memcpy(iter, WHITE, strlen(WHITE));
            iter += strlen(WHITE);
            memcpy(iter, &m, sizeof(m));
            iter += sizeof(m);
            memcpy(iter, TCP_END, strlen(TCP_END));
            iter += strlen(TCP_END);

            ssize_t answerLen = iter - answer;

            rezSend = send(fd2, answer, answerLen, 0);
            if (rezSend < answerLen)
            {
                perror("REGOK sending failure");
                return -1;
            }
            isFound = 1;
        }
        gameEl = gameEl->next;
    }
    if (0 == isFound)
    {
        // REGNO*** -> inscription refused
        sprintf(answer, "%s%s%c", REGNO, TCP_END, '\0');
        rezSend = send(fd2, answer, strlen(answer), 0);
        if (rezSend < strlen(answer))
        {
            perror("REGNO sending failure");
            return -1;
        }
    }

    return rezSend;
}

//-> UNREG m*** -> unsubscribe from game m
ssize_t processUNREG(struct stGamerContext *gContext, char *bufer, ssize_t rezSend, char *answer)
{
    int fd2 = gContext->pGamer->fd2;
    char *iter = (char *)bufer;

    uint8_t m = 0;
    memcpy(&m, iter, sizeof(m));
    // printf("asked game : %d\n", m);

    // find game m
    pthread_mutex_lock(&gContext->pServerCtx->serverLock);

    struct element_t *gameEl = gContext->pServerCtx->games->first;
    unsigned int isFound = 0;

    while (gameEl)
    {
        struct stGame *game = (struct stGame *)(gameEl->data);

        if (game->idGame == m)
        {
            // find this gamer
            struct element_t *gamerEl = game->gamers->first;
            struct stGamer *gamer = NULL;
            while (gamerEl)
            {
                gamer = (struct stGamer *)(gamerEl->data);
                if (gamer->id == gContext->pGamer->id)
                {
                    removeEl(game->gamers, gamerEl->data);
                    FREE_MEM(gamer);

                    // UNROK m*** -> unsubscribe OK
                    iter = answer;

                    memcpy(iter, UNROK, strlen(UNROK));
                    iter += strlen(UNROK);
                    memcpy(iter, WHITE, strlen(WHITE));
                    iter += strlen(WHITE);
                    memcpy(iter, &m, sizeof(m));
                    iter += sizeof(m);
                    memcpy(iter, TCP_END, strlen(TCP_END));
                    iter += strlen(TCP_END);

                    ssize_t answerLen = iter - answer;

                    rezSend = send(fd2, answer, answerLen, 0);
                    if (rezSend < answerLen)
                    {
                        perror("UNROK sending failure");
                        return -1;
                    }
                }
                gamerEl = gamerEl->next;
            }
        }
        gameEl = gameEl->next;
    }
    pthread_mutex_unlock(&gContext->pServerCtx->serverLock);
    if (0 == isFound)
    {
        // DUNNO*** -> m refused
        sprintf(answer, "%s%s%c", DUNNO, TCP_END, '\0');
        rezSend = send(fd2, answer, strlen(answer), 0);
        if (rezSend < strlen(answer))
        {
            perror("DUNNO sending failure");
            return -1;
        }
    }
    return rezSend;
}

ssize_t processSIZE_(struct stGamerContext *gContext, char *bufer, ssize_t rezSend, char *answer)
{
    //-> SIZE? m*** -> dde taille du labyrinthe
    //               SIZE! m h w*** -> h hauteur, w largeur
    //               DUNNO*** -> m inconnu
    return 0;
}

//-> LIST? m*** -> liste des joueurs inscrits à une partie
ssize_t processLIST_(struct stGamerContext *gContext, char *bufer, ssize_t rezSend, char *answer)
{
    int fd2 = gContext->pGamer->fd2;
    char *iter = (char *)bufer;

    uint8_t m = *(uint8_t *)iter;
    int iFound = 0;

    pthread_mutex_lock(&gContext->pServerCtx->serverLock);

    // find the game m
    struct element_t *gameEl = gContext->pServerCtx->games->first;

    printf("%s asked game : %d, games count %d\n", __FUNCTION__, m, gContext->pServerCtx->games->count);

    while (gameEl)
    {
        struct stGame *game = (struct stGame *)(gameEl->data);

        if (game->idGame == m)
        {
            // compose LIST! m s*** -> s = nb de joueurs
            iter = answer;
            memcpy(iter, LIST_E, strlen(LIST_E));
            iter += strlen(LIST_E);
            memcpy(iter, &game->idGame, sizeof(game->idGame));
            iter += sizeof(game->idGame);
            memcpy(iter, WHITE, strlen(WHITE));
            iter += strlen(WHITE);
            memcpy(iter, &game->gamers->count, sizeof(game->gamers->count));
            iter += sizeof(game->gamers->count);
            memcpy(iter, TCP_END, strlen(TCP_END));
            iter += strlen(TCP_END);

            ssize_t answerLen = iter - answer;

            // send
            rezSend = send(fd2, answer, answerLen, 0);
            if (rezSend < answerLen)
            {
                perror("LIST! sending failure");
                return -1;
            }

            // specify all gamers
            struct element_t *gamerEl = game->gamers->first;

            printf("%s games count has been sent, gamers %d\n", __FUNCTION__, game->gamers->count);

            unsigned int count = 1;
            while (gamerEl)
            {
                struct stGamer *gamer = (struct stGamer *)(gamerEl->data);

                // PLAYR id***-> s times this message, id = id of gamer
                sprintf(answer, "%s %s%s", PLAYR, gamer->id, TCP_END);

                printf("%s gamer %d - %s, TCP: {%s}\n", __FUNCTION__, count, gamer->id, answer);

                rezSend = send(fd2, answer, strlen(answer), 0);
                if (rezSend < strlen(answer))
                {
                    perror("PLAYR id*** sending failure");
                    return -1;
                }
                count++;
                gamerEl = gamerEl->next;
            }
            iFound = 1;
            break;
        }
        gameEl = gameEl->next;
    }

    pthread_mutex_unlock(&gContext->pServerCtx->serverLock);

    if (0 == iFound)
    {
        // DUNNO***
        sprintf(answer, "%s%s%c", DUNNO, TCP_END, '\0');
        rezSend = send(fd2, answer, strlen(answer), 0);
        if (rezSend < strlen(answer))
        {
            perror("DUNNO sending failure");
            return -1;
        }
    }
    return rezSend;
}

//-> GAME?*** -> ask list of games non-launched
ssize_t processGAME_(struct stGamerContext *gContext, ssize_t rezSend, char *answer)
{
    int fd2 = gContext->pGamer->fd2;

    pthread_mutex_lock(&gContext->pServerCtx->serverLock);

    // find game m
    struct element_t *gameEl = gContext->pServerCtx->games->first;
    struct stGame *game = NULL;

    unsigned int count = 0;

    while (gameEl)
    {
        game = (struct stGame *)(gameEl->data);

        if (!game->isLaunched)
        {
            count++;
            // compose LIST! m s*** -> s = nb de joueurs
            char *iter = (char *)answer;

            memcpy(iter, LIST_E, strlen(LIST_E));
            iter += strlen(LIST_E);
            memcpy(iter, &game->idGame, sizeof(game->idGame));
            iter += sizeof(game->idGame);
            memcpy(iter, WHITE, strlen(WHITE));
            iter += strlen(WHITE);
            memcpy(iter, &game->gamers->count, sizeof(game->gamers->count));
            iter += sizeof(game->gamers->count);
            memcpy(iter, TCP_END, strlen(TCP_END));
            iter += strlen(TCP_END);

            ssize_t answerLen = iter - answer;

            // send
            rezSend = send(fd2, answer, answerLen, 0);
            if (rezSend < answerLen)
            {
                perror("LIST! sending failure");
                return -1;
            }

            // specify all gamers
            struct element_t *gamerEl = game->gamers->first;

            while (gamerEl)
            {
                struct stGamer *gamer = (struct stGamer *)(gamerEl->data);

                // PLAYR id***-> s times this message, id = id of gamer
                sprintf(answer, "%s %s%s%c", PLAYR, gamer->id, TCP_END, '\0');

                rezSend = send(fd2, answer, strlen(answer), 0);
                if (rezSend < strlen(answer))
                {
                    perror("PLAYR id*** sending failure");
                    return -1;
                }
                gamerEl = gamerEl->next;
            }
        }
        gameEl = gameEl->next;
    }
    pthread_mutex_unlock(&gContext->pServerCtx->serverLock);

    if (0 == count)
    {
        // DUNNO***
        sprintf(answer, "%s%s%c", DUNNO, TCP_END, '\0');
        rezSend = send(fd2, answer, strlen(answer), 0);
        if (rezSend < strlen(answer))
        {
            perror("DUNNO sending failure");
            return -1;
        }
    }
    return rezSend;
}

//////////////////////////// Auxiliary functions /////////////////////////////////

// create new game and fill folowing fields: idGame, port.
// return this new game
struct stGame *createGame(struct stGamerContext *gContext, char *bufer)
{
    //create new stGame
    struct stGame *game = (struct stGame *)malloc(sizeof(struct stGame));
    if (!game)
    {
        perror("malloc stGame");
        return NULL;
    }
    memset(game, 0, sizeof(struct stGame));
    game->isLaunched = 0; // is done abouve
    pthread_mutex_init(&game->gameLock, NULL);

    pthread_mutex_lock(&gContext->pServerCtx->serverLock);
    game->idGame = ++gContext->pServerCtx->lastGameId;
    pthread_mutex_unlock(&gContext->pServerCtx->serverLock);

    //create list of gamers and labirinth
    game->gamers = (struct listElements_t *)malloc(sizeof(struct listElements_t));
    game->labirinth = (struct stLabirinth *)malloc(sizeof(struct stLabirinth));
    if (!game->gamers || !game->labirinth)
    {
        perror("malloc gamers or labirinth in createGame");
        return NULL;
    }
    memset(game->gamers, 0, sizeof(struct listElements_t));
    memset(game->labirinth, 0, sizeof(struct stLabirinth));

    //create list of ghosts and pointer to stGrid
    game->labirinth->ghosts = (struct listElements_t *)malloc(sizeof(struct listElements_t));
    game->labirinth->grid = (struct stCell *)malloc(sizeof(struct stCell));
    if (!game->labirinth->ghosts || !game->labirinth->grid)
    {
        perror("malloc list of ghosts or grid in createGame");
        return NULL;
    }
    memset(game->labirinth->ghosts, 0, sizeof(struct listElements_t));
    memset(game->labirinth->grid, 0, sizeof(struct stCell));
    pthread_mutex_init(&game->labirinth->labLock, NULL);
    pthread_mutex_init(&game->labirinth->grid->cellLock, NULL);

    return game;
}

// free memory of struct game
void freeGame(struct stGame *pGame)
{
    if (pGame)
    {
        // free gamers
        struct element_t *gamerEl = pGame->gamers->first;
        while (gamerEl)
        {
            struct stGamer *gamer = (struct stGamer *)(gamerEl->data);
            pthread_mutex_destroy(&gamer->gamerLock);
            FREE_MEM(gamer);
            gamerEl = gamerEl->next;
        }

        FREE_MEM(pGame->gamers);

        // free labirinth
        if (pGame->labirinth)
        {
            struct element_t *ghostEl = pGame->labirinth->ghosts->first;
            while (ghostEl)
            {
                struct stGhost *ghost = (struct stGhost *)(ghostEl->data);
                pthread_mutex_destroy(&ghost->ghostLock);
                FREE_MEM(ghost);
                ghostEl = ghostEl->next;
            }
            pthread_mutex_destroy(&pGame->labirinth->labLock);
            FREE_MEM(pGame->labirinth->ghosts);

            FREE_MEM(pGame->labirinth->grid);
            FREE_MEM(pGame->labirinth);
        }

        // free game
        FREE_MEM(pGame);
    }
}

// recieve till ending = "***" or "+++" will be recieved
// return -1 if error, 0 if session has been closed, else the number of bytes received
size_t recieveMessage(int fd, char *bufer, char *ending)
{
#if defined(PRINT_PROTOCOL)
    char *path = "./test.bin";
#endif

    size_t got = 0;
    ssize_t rezRecv = -1;
    const ssize_t szEnding = strlen(ending);

    while (got < BUF_SIZE)
    {
        rezRecv = recv(fd, bufer + got, /*BUF_SIZE - got*/ 1, 0);
        if (rezRecv < 0) //error
        {
            // termine with error
            perror("recieve failure");
            return -1;
        }
        else if (rezRecv == 0) // server has disconnected
        {
            // termine this session
            printf("Client has close session!\n");
            return 0;
        }
        got += rezRecv;

        if (got > szEnding)
        {
            // check last 3 bytes
            if (0 == strncmp(bufer + got - szEnding, ending, szEnding))
            {
                bufer[got] = 0;
                printf("MSG ... {%s}\n", bufer);
#if defined(PRINT_PROTOCOL)

                int file = open(path, O_CREAT | O_RDWR | O_APPEND,
                                S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH);
                if (file < 0)
                {
                    perror("open failure");
                }

                ssize_t r = write(file, bufer, got);
                if (r < got)
                {
                    perror("write failure");
                }
#endif
                CLOSE(file);
                return got;
            }
        }
    }

    perror("recieved more than buffer");

    return -1;
}

// create new gamer
struct stGamer *createGamer(int32_t socket, uint32_t uIpv4)
{
    struct stGamer *newGamer = (struct stGamer *)malloc(sizeof(struct stGamer));
    if (!newGamer)
    {
        perror("malloc gamer failure");
        return NULL;
    }

    memset(newGamer, 0, sizeof(struct stGamer));
    newGamer->fd2 = socket;
    newGamer->ipAddress = uIpv4; // TODO check that need htons
    pthread_mutex_init(&newGamer->gamerLock, NULL);

    return newGamer;
}

// free memory of strauct gamer
void freeGamer(struct stGamer *pGamer)
{
    if (pGamer)
    {
        pthread_mutex_destroy(&pGamer->gamerLock);
        CLOSE(pGamer->fd2);
        FREE_MEM(pGamer);
    }
}

// add gamer to list of gamers
void addGamer(struct stGamerContext *gContext, struct stGame *game, struct stGamer *gamer)
{
    pthread_mutex_lock(&gContext->pServerCtx->serverLock);
    pushLast(game->gamers, gContext->pGamer);
    pthread_mutex_unlock(&gContext->pServerCtx->serverLock);
}

// remove gamer from list of gamers
void removeGamer(struct stGamerContext *gContext, struct stGame *game, struct stGamer *gamer)
{
    if ((!gContext) || (!game) || (!gamer))
    {
        perror("removeGamer() args error!");
        return;
    }

    pthread_mutex_lock(&gContext->pServerCtx->serverLock);
    struct element_t *pIter = game->gamers->first;
    int iFound = 0;
    while (pIter)
    {
        if (((struct stGamer *)pIter->data) == gamer)
        {
            removeEl(game->gamers, pIter);
            iFound = 1;
            break;
        }
        pIter = pIter->next;
    }
    pthread_mutex_unlock(&gContext->pServerCtx->serverLock);

    if (!iFound)
    {
        perror("removeGamer() failed, gamer not found!");
    }
}

// add game to list of games
void addGame(struct stGamerContext *gContext, struct stGame *newGame)
{
    pthread_mutex_lock(&gContext->pServerCtx->serverLock);
    pushLast(gContext->pServerCtx->games, newGame);
    pthread_mutex_unlock(&gContext->pServerCtx->serverLock);
}

// remove game from list of games
void removeGame(struct stGamerContext *gContext, struct stGame *game)
{
    if ((!gContext) || (!game))
    {
        perror("removeGame() args error!");
        return;
    }

    pthread_mutex_lock(&gContext->pServerCtx->serverLock);

    struct element_t *pIter = gContext->pServerCtx->games->first;
    int iFound = 0;
    while (pIter)
    {
        if (((struct stGame *)pIter->data) == game)
        {
            removeEl(gContext->pServerCtx->games, pIter);
            iFound = 1;
            break;
        }
        pIter = pIter->next;
    }

    pthread_mutex_unlock(&gContext->pServerCtx->serverLock);

    if (!iFound)
    {
        perror("removeGame() failed, game not found!");
    }
}

void printGamers(struct stGamerContext *gContext, const char *pCaller)
{
    if (!gContext)
    {
        perror("printGamers() args error!");
        return;
    }

    pthread_mutex_lock(&gContext->pServerCtx->serverLock);

    printf("{%s} Gamers++++++++++++++++++\n", pCaller);

    struct element_t *pIterGame = gContext->pServerCtx->games->first;
    while (pIterGame)
    {
        struct stGame *pGame = (struct stGame *)pIterGame->data;
        struct element_t *pIterGameR = pGame->gamers->first;
        while (pIterGameR)
        {
            struct stGamer *pGameR = (struct stGamer *)pIterGameR->data;
            printf("Game %d, gamer {%s}\n", (int)pGame->idGame, pGameR->id);
            pIterGameR = pIterGameR->next;
        }

        pIterGame = pIterGame->next;
    }
    printf("{%s} Gamers----------------\n", pCaller);

    pthread_mutex_unlock(&gContext->pServerCtx->serverLock);
}