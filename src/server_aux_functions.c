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
    context->tcpListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (context->tcpListenSocket < 0)
    {
        perror("P: cannot create socket");
        rez = -1;
        goto lExit;
    }

    rez = bind(context->tcpListenSocket, (struct sockaddr *)&context->sockAddress, sizeof(struct sockaddr_in));
    if (rez < 0)
    {
        perror("P: bind failure");
        goto lExit;
    }

    rez = listen(context->tcpListenSocket, 0);
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

    int tcpSocket = -1;
    socklen_t len = sizeof(context->sockAddress);

    // make accept
    tcpSocket = accept(context->tcpListenSocket, (struct sockaddr *)&context->sockAddress, &len);
    if (tcpSocket < 0) // error
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

        pGamerContext->serverCtx = context;
        pGamerContext->player = createPlayer(tcpSocket, atoi(inet_ntoa(context->sockAddress.sin_addr))); // TODO check that need htons

        if (!pGamerContext->player)
        {
            perror("malloc gamer failure");
            rez = EXIT_FAILURE;
            goto lExit;
        }

        // create thread of gamer for routine_com
        int phRez = pthread_create(&th, NULL, routine_com, (void *)pGamerContext);
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

// routine_com with client
void *routine_com(void *args)
{
    // // declaration & initialisation
    struct stGamerContext *pGamerContext = (struct stGamerContext *)(args);
    int                    tcpSocket    = pGamerContext->player->tcpSocket;
    struct stRawMessage    stMsg;

    sendGames(pGamerContext);

    // gamer coomunication cycle
    while (1)
    {
        stMsg.msg = eUdpMsgMaxValue;

        int iMsgRes = tcpGetMsg(tcpSocket, &pGamerContext->player->stTcpBuf, &stMsg);
        if (iMsgRes < 0)
        {
            fprintf(stderr, "{%s} Socket/Communication error, close connection for player [%s]!\n", 
                    __FUNCTION__, pGamerContext->player->id);
            break;
        }
        else if (iMsgRes == 0)
        {
            continue;
        }

        if (stMsg.msg == eTcpMsgNEWPL)
        {
            processNEWPL(pGamerContext, (uint8_t*)stMsg.msgRaw);
        }
        else if (stMsg.msg == eTcpMsgREGIS)
        {
            processREGIS(pGamerContext, (uint8_t*)stMsg.msgRaw);
        }
        else if (stMsg.msg == eTcpMsgUNREG)
        {
            processUNREG(pGamerContext, (uint8_t*)stMsg.msgRaw);
        }
        else if (stMsg.msg == eTcpMsgSIZEQ)
        {
            processSIZE_(pGamerContext, (uint8_t*)stMsg.msgRaw);
        }
        else if (stMsg.msg == eTcpMsgLISTQ)
        {
            processLIST_(pGamerContext, (uint8_t*)stMsg.msgRaw);
        }
        else if (stMsg.msg == eTcpMsgGAMEQ)
        {
            sendGames(pGamerContext);
        }

        // else if (0 == strcmp(req, GAMES))
        // {
        //     processGAMES(context, bufer, rezSend, answer);
        // }

        //        - if START recieved from all lstPlayers :
        //                      + create thread
        //                      + create UDP
        //                      + launch engine on it with this stGame
    }

    //TODO: uncomment
    //removeGamePlayer(pGamerContext, pGamerContext->player);
    //freePlayer(pGamerContext->player);

    printf("{%s} Close player [%s] communication thread!\n", __FUNCTION__, pGamerContext->player->id);
    return NULL;
}

//////////////////////////// Process functions //////////////////////////////
////           return -1 if error, else nmber of sent bytes      ////////

// send info about all lstGames and about each game
bool sendGames(struct stGamerContext *gContext)
{
    pthread_mutex_lock(&gContext->serverCtx->serverLock);
    int      tcpSocket  = gContext->player->tcpSocket;
    uint32_t uGamecCount = (uint32_t)(gContext->serverCtx->lstGames->count - (size_t)gContext->serverCtx->countStarted);
    bool     bRet        = true;

    //send info about all lstGames
    if (!sendMsg(tcpSocket, eTcpMsgGAMES, uGamecCount))
    {
        perror("GAMES<n>*** sending failure");
        bRet = false;
    }

    if (bRet)
    {
        //send info about each game
        struct element_t *gameEl = gContext->serverCtx->lstGames->first;
        struct stGame *game = NULL;
        while (gameEl)
        {
            game = (struct stGame *)(gameEl->data);
            if (0 == game->isLaunched)
            {      
                if (!sendMsg(tcpSocket, eTcpMsgOGAME, (uint32_t)game->idGame, (uint32_t)game->lstPlayers->count))
                {
                    perror("OGAME<m><s>*** for sending failure");
                    bRet = false;
                    break;
                }
            }
            gameEl = gameEl->next;
        }
    }

    pthread_mutex_unlock(&gContext->serverCtx->serverLock);

    return bRet;
}

//-> NEWPL id port*** -> ask create game
bool processNEWPL(struct stGamerContext *gContext, uint8_t *bufer)
{
    int tcpSocket = gContext->player->tcpSocket;

    // create new game
    struct stGame *newGame = createGame(gContext);
    if (newGame) //REGOK m*** -> inscription OK
    {
        // add this game to list of lstGames
        addGame(gContext, newGame); //ADD LAST

        uint32_t port;
        if (2 != scanMsg(bufer, eTcpMsgNEWPL, gContext->player->id, &port))
        {
            fprintf(stderr, "{%s} NEWPL scan error!", __FUNCTION__);
            return false;
        }
        gContext->player->portUDP = (uint16_t)port;

        printf("{%s} receive gamer ID {%s}\n", __FUNCTION__, gContext->player->id);

        // add this gamer to list of lstPlayers
        addGamePlayer(gContext, newGame, gContext->player); //ADD LAST
        
        if (!sendMsg(tcpSocket, eTcpMsgREGOK, (uint32_t)newGame->idGame))
        {
            fprintf(stderr, "{%s} REGOK sending failure!", __FUNCTION__);
            return false;
        }
    }
    else // REGNO***
    {
        if (!sendMsg(tcpSocket, eTcpMsgREGNO))
        {
            fprintf(stderr, "{%s} REGNO sending failure!", __FUNCTION__);
            return false;
        }
    }

    return true;
}

//-> REGIS id port m*** -> subscribe to game m
bool processREGIS(struct stGamerContext *gContext, uint8_t *bufer)
{
    int      tcpSocket = gContext->player->tcpSocket;
    bool     bRet       = false;
    uint32_t port;
    uint32_t m;

    if (3 == scanMsg(bufer, eTcpMsgREGIS, gContext->player->id, &port, &m))
    {
        gContext->player->portUDP = (uint16_t)port;

        printf("{%s} asked game : %u\n", __FUNCTION__, m);

        pthread_mutex_lock(&gContext->serverCtx->serverLock); //protect lstGames access

        // add gamer into list of lstPlayers of this game
        struct element_t *gameEl  = gContext->serverCtx->lstGames->first;
        uint32_t          isFound = 0;
        while ((gameEl) && (!isFound))
        {
            struct stGame *game = (struct stGame *)(gameEl->data);

            pthread_mutex_lock(&game->gameLock);
            if ((game->idGame == m) && (!game->isLaunched)) // REGOK m*** -> inscription OK
            {
                bRet = true;
                if (addGamePlayer(gContext, game, gContext->player))
                {
                    isFound = 1;
                    if (!sendMsg(tcpSocket, eTcpMsgREGOK, m))
                    {
                        fprintf(stderr, "{%s} REGOK sending failure!", __FUNCTION__);
                        bRet = false;
                    }
                }
            }
            pthread_mutex_unlock(&game->gameLock);
            gameEl = gameEl->next;
        }

        pthread_mutex_unlock(&gContext->serverCtx->serverLock); //protect lstGames access

        if (0 == isFound)
        {
            bRet = true;
            if (!sendMsg(tcpSocket, eTcpMsgREGNO))
            {
                fprintf(stderr, "{%s} REGNO sending failure!", __FUNCTION__);
                bRet = false;
            }
        }
    }
    else
    {
        fprintf(stderr, "{%s} REGIS scan error!", __FUNCTION__);
    }

    return bRet;
}

//-> UNREG m*** -> unsubscribe from game m
bool processUNREG(struct stGamerContext *gContext, uint8_t *bufer)
{   
    int tcpSocket = gContext->player->tcpSocket;
    bool bRet = false;
    if (gContext->player->pGame)
    {
        uint32_t m = gContext->player->pGame->idGame;
        bRet = removeGamePlayer(gContext, gContext->player);

        if (!sendMsg(tcpSocket, eTcpMsgUNROK, m))
        {
            fprintf(stderr, "{%s} UNROK sending failure!", __FUNCTION__);
            bRet = false;
        }
    }
    else
    {
        bRet = true;
        if (!sendMsg(tcpSocket, eTcpMsgDUNNO))
        {
            fprintf(stderr, "{%s} DUNNO sending failure!", __FUNCTION__);
            bRet = false;
        }
    }
    return bRet;
}

bool processSIZE_(struct stGamerContext *gContext, uint8_t *bufer)
{
    pthread_mutex_lock(&gContext->serverCtx->serverLock);   

    bool     bRet       = false;
    int      tcpSocket = gContext->player->tcpSocket;
    uint32_t m;
    if (1 == scanMsg(bufer, eTcpMsgSIZEQ, &m))
    {
        // find the game m
        struct element_t *gameEl = gContext->serverCtx->lstGames->first;
        int isFound = 0;

        while ((gameEl) && (!isFound))
        {
            struct stGame *game = (struct stGame *)(gameEl->data);
            pthread_mutex_lock(&game->gameLock);   

            if (game->idGame == m)
            {
                bRet = true;
                if (!sendMsg(tcpSocket, eTcpMsgSIZEA, m, (uint32_t)game->labirinth.heigh, (uint32_t)game->labirinth.width))
                {
                    fprintf(stderr, "{%s} SIZE! sending failure!", __FUNCTION__);
                    bRet = false;
                }

                isFound = 1;
            }
            pthread_mutex_unlock(&game->gameLock);

            gameEl = gameEl->next;
        }

        if (!isFound)
        {
            bRet = true;
            if (!sendMsg(tcpSocket, eTcpMsgDUNNO))
            {
                fprintf(stderr, "{%s} DUNNO sending failure!", __FUNCTION__);
                bRet = false;
            }
        }
    }

    pthread_mutex_unlock(&gContext->serverCtx->serverLock);
    return bRet;
}

//-> LIST? m*** -> liste des joueurs inscrits à une partie
bool processLIST_(struct stGamerContext *gContext, uint8_t *bufer)
{
    pthread_mutex_lock(&gContext->serverCtx->serverLock);   

    int      tcpSocket = gContext->player->tcpSocket;
    bool     bRet       = false;
    int      isFound    = 0;
    uint32_t m;

    if (1 == scanMsg(bufer, eTcpMsgLISTQ, &m))
    {
        // find the game m
        struct element_t *gameEl = gContext->serverCtx->lstGames->first;

        while ((gameEl) && (!isFound))
        {
            struct stGame *game = (struct stGame *)(gameEl->data);

            pthread_mutex_lock(&game->gameLock);   
            if (game->idGame == m)
            {
                bRet    = true;
                isFound = 1;
                if (!sendMsg(tcpSocket, eTcpMsgLISTA, m, (uint32_t)game->lstPlayers->count))
                {
                    fprintf(stderr, "{%s} LISTA sending failure!", __FUNCTION__);
                    bRet = false;
                }

                if (bRet)
                {
                    // specify all lstPlayers
                    struct element_t *gamerEl = game->lstPlayers->first;
                    while (gamerEl)
                    {
                        struct stPlayer *gamer = (struct stPlayer *)(gamerEl->data);

                        if (!sendMsg(tcpSocket, eTcpMsgPLAYR, gamer->id))
                        {
                            fprintf(stderr, "{%s} PLAYR sending failure!", __FUNCTION__);
                            bRet = false;
                        }
                        gamerEl = gamerEl->next;
                    }
                }
            }
            pthread_mutex_unlock(&game->gameLock);   
        }
        gameEl = gameEl->next;
    }

    pthread_mutex_unlock(&gContext->serverCtx->serverLock);

    if (0 == isFound)
    {
        bRet = true;
        if (!sendMsg(tcpSocket, eTcpMsgDUNNO))
        {
            fprintf(stderr, "{%s} DUNNO sending failure!", __FUNCTION__);
            bRet = false;
        }
    }
    return bRet;
}

//////////////////////////// Auxiliary functions /////////////////////////////////

// create new game and fill folowing fields: idGame, port.
// return this new game
struct stGame *createGame(struct stGamerContext *gContext)
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
    pthread_mutexattr_t Attr;
    pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&game->gameLock, &Attr);

    pthread_mutex_lock(&gContext->serverCtx->serverLock);
    game->idGame = ++gContext->serverCtx->lastGameId;
    pthread_mutex_unlock(&gContext->serverCtx->serverLock);

    //create list of lstPlayers and labirinth
    game->lstPlayers = (struct listElements_t *)malloc(sizeof(struct listElements_t));
    if (!game->lstPlayers)
    {
        perror("malloc lstPlayers or labirinth in createGame");
        return NULL;
    }
    memset(game->lstPlayers, 0, sizeof(struct listElements_t));

    game->labirinth.width = (rand() % 96) + 32;
    game->labirinth.heigh = (rand() % 96) + 32;
    game->labirinth.maze  = maze_create(game->labirinth.width, game->labirinth.heigh);

    //create list of ghosts and pointer to stGrid
    game->labirinth.ghosts = (struct listElements_t *)malloc(sizeof(struct listElements_t));
    if (!game->labirinth.ghosts)
    {
        perror("malloc list of ghosts or grid in createGame");
        return NULL;
    }
    memset(game->labirinth.ghosts, 0, sizeof(struct listElements_t));

    uint8_t bGhostsCount = game->labirinth.width & 0xFF;
    while (bGhostsCount--)
    {
        struct stGhost *pGhost = (struct stGhost *)malloc(sizeof(struct stGhost));
        memset(pGhost, 0, sizeof(sizeof(struct stGhost)));
        pGhost->id = bGhostsCount;
        do {
            pGhost->x = rand() % game->labirinth.width;
            pGhost->y = rand() % game->labirinth.heigh;
        } while (MAZE_WALL == game->labirinth.maze[(uint32_t)pGhost->y * (uint32_t)game->labirinth.width + (uint32_t)pGhost->y]);

        pushFirst(game->labirinth.ghosts, pGhost);
    }

    return game;
}

// free memory of struct game
void freeGame(struct stGame *pGame)
{
    if (pGame)
    {
        pthread_mutex_lock(&pGame->gameLock);

        // free lstPlayers
        while (pGame->lstPlayers->first)
        {
            //REMARK: player context belongs to communication thread and will be released in there
            ((struct stPlayer*)(pGame->lstPlayers->first->data))->pGame = NULL;
            removeEl(pGame->lstPlayers, pGame->lstPlayers->first);
        }

        FREE_MEM(pGame->lstPlayers);

        while (pGame->labirinth.ghosts->first)
        {
            struct stGhost *ghost = (struct stGhost *)(pGame->labirinth.ghosts->first->data);
            FREE_MEM(ghost);
            removeEl(pGame->labirinth.ghosts, pGame->labirinth.ghosts->first);
        }

        FREE_MEM(pGame->labirinth.ghosts);
        FREE_MEM(pGame->labirinth.maze);

        // free game
        pthread_mutex_unlock(&pGame->gameLock);

        pthread_mutex_destroy(&pGame->gameLock);
        FREE_MEM(pGame);
    }
}

// create new gamer
struct stPlayer *createPlayer(int32_t socket, uint32_t uIpv4)
{
    struct stPlayer *newPlayer = (struct stPlayer *)malloc(sizeof(struct stPlayer));
    if (!newPlayer)
    {
        perror("malloc gamer failure");
        return NULL;
    }

    memset(newPlayer, 0, sizeof(struct stPlayer));

    newPlayer->tcpSocket  = socket;
    newPlayer->ipAddress   = uIpv4; //TODO check that need htons
    newPlayer->pGame       = NULL;

    return newPlayer;
}

// free memory of strauct gamer
void freePlayer(struct stPlayer *player)
{
    if (player)
    {
        CLOSE(player->tcpSocket);
        FREE_MEM(player);
    }
}

// add gamer to list of lstPlayers
bool addGamePlayer(struct stGamerContext *gContext, struct stGame *game, struct stPlayer *pNewPlayer)
{
    pthread_mutex_lock(&game->gameLock);

    struct element_t *playerEl = game->lstPlayers->first;
    bool bRet = true;

    while (playerEl)
    {
        struct stPlayer *player = (struct stPlayer *)(playerEl->data);

        if (0 == memcmp(player->id, pNewPlayer->id,USER_ID_LEN))
        {
            bRet = false;
            break;
        }
        playerEl = playerEl->next;
    }
    if (bRet)
    {
        pushLast(game->lstPlayers, pNewPlayer);
        pNewPlayer->pGame = game;
    }

    pthread_mutex_unlock(&game->gameLock);
    return bRet;
}

// remove gamer from list of lstPlayers
bool removeGamePlayer(struct stGamerContext *gContext, struct stPlayer *player)
{
    if ((!gContext) || (!player->pGame) || (!player))
    {
        perror("removeGamePlayer() args error!");
        return false;
    }

    pthread_mutex_lock(&player->pGame->gameLock);
    struct stGame    *pCurrentGame = player->pGame;
    struct element_t *pIter        = player->pGame->lstPlayers->first;
    bool              bFound       = false;
    while (pIter)
    {
        if (((struct stPlayer *)pIter->data) == player)
        {
            removeEl(player->pGame->lstPlayers, pIter);
            player->pGame = NULL;
            bFound = true;
            break;
        }
        pIter = pIter->next;
    }

    size_t szPlayersCount = player->pGame->lstPlayers->count;
    pthread_mutex_unlock(&player->pGame->gameLock);

    if (  (!szPlayersCount) && (bFound) )
    {
        if (removeGame(gContext, pCurrentGame, false))
        {
            freeGame(pCurrentGame);
        }
    }

    if (!bFound)
    {
        perror("removeGamePlayer() failed, gamer not found!");
    }

    return bFound;
}

// add game to list of lstGames
void addGame(struct stGamerContext *gContext, struct stGame *newGame)
{
    pthread_mutex_lock(&gContext->serverCtx->serverLock);
    pushLast(gContext->serverCtx->lstGames, newGame);
    pthread_mutex_unlock(&gContext->serverCtx->serverLock);
}

// remove game from list of lstGames
bool removeGame(struct stGamerContext *gContext, struct stGame *game, bool bForce)
{
    if ((!gContext) || (!game))
    {
        perror("removeGame() args error!");
        return false;
    }

    pthread_mutex_lock(&gContext->serverCtx->serverLock);

    struct element_t *pIter = gContext->serverCtx->lstGames->first;
    bool   bRemoved = false;
    while (pIter)
    {
        if (((struct stGame *)pIter->data) == game)
        {
            pthread_mutex_lock(&game->gameLock);
            if ((!game->lstPlayers->count) || (bForce))
            {
                removeEl(gContext->serverCtx->lstGames, pIter);
                bRemoved = true;
            }
            pthread_mutex_unlock(&game->gameLock);

            break;
        }
        pIter = pIter->next;
    }

    pthread_mutex_unlock(&gContext->serverCtx->serverLock);
    return bRemoved;
}

void printPlayers(struct stGamerContext *gContext, const char *pCaller)
{
    if (!gContext)
    {
        perror("printPlayers() args error!");
        return;
    }

    printf("{%s} Gamers++++++++++++++++++\n", pCaller);
    pthread_mutex_lock(&gContext->serverCtx->serverLock);
    struct element_t *pIterGame = gContext->serverCtx->lstGames->first;
    while (pIterGame)
    {
        struct stGame *pGame = (struct stGame *)pIterGame->data;
        struct element_t *pIterGameR = pGame->lstPlayers->first;
        while (pIterGameR)
        {
            struct stPlayer *pGameR = (struct stPlayer *)pIterGameR->data;
            printf("Game %d, gamer {%s}\n", (int)pGame->idGame, pGameR->id);
            pIterGameR = pIterGameR->next;
        }

        pIterGame = pIterGame->next;
    }
    pthread_mutex_unlock(&gContext->serverCtx->serverLock);
    printf("{%s} Gamers----------------\n", pCaller);
}