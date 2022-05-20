#include "server_aux_functions.h"
#include "engine.h"
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
        log_error("P: cannot create socket");
        rez = -1;
        goto lExit;
    }

    int enable = 1;
    setsockopt(context->tcpListenSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    rez = bind(context->tcpListenSocket, (struct sockaddr *)&context->sockAddress, sizeof(struct sockaddr_in));
    if (rez < 0)
    {
        log_error("P: bind failure");
        goto lExit;
    }

    rez = listen(context->tcpListenSocket, 0);
    if (rez < 0)
    {
        log_error("P:listen failure");
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
        log_error("P: accept failure");
        rez = EXIT_FAILURE;
    }
    else // session started
    {
        // create gamer and gamer's context
        struct stGamerContext *pGamerContext = (struct stGamerContext *)malloc(sizeof(struct stGamerContext));
        if (!pGamerContext)
        {
            log_fatal("malloc gamer context failure");
            rez = EXIT_FAILURE;
            goto lExit;
        }

        pGamerContext->serverCtx = context;
        pGamerContext->player = createPlayer(tcpSocket, ntohl(context->sockAddress.sin_addr.s_addr)); // TODO check that need htons

        if (!pGamerContext->player)
        {
            log_fatal("malloc gamer failure");
            rez = EXIT_FAILURE;
            goto lExit;
        }

        // create thread of gamer for routine_com
        int phRez = pthread_create(&th, NULL, routine_com, (void *)pGamerContext);
        if (phRez != 0) // error
        {
            log_fatal("P: pthread_create failure");
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
            log_info("Socket/Communication error, close connection for player [%s]!\n", pGamerContext->player->id);
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
        else if (stMsg.msg == eTcpMsgSTART)
        {
            processSTART(pGamerContext, (uint8_t*)stMsg.msgRaw);
        }
        else if (stMsg.msg == eTcpMsgGLISQ)
        {
            if (!processGLIS(pGamerContext))
            {
                sendMsg(tcpSocket, eTcpMsgGOBYE);
                break;
            }
        }
        else if (stMsg.msg == eTcpMsgIQUIT)
        {
            if (sendMsg(tcpSocket, eTcpMsgGOBYE))
            {
                break;
            }
        }
        else if (stMsg.msg == eTcpMsgMALLQ)
        {
            if (!processMALL_(pGamerContext, (uint8_t*)stMsg.msgRaw))
            {
                break;
            }
        }
        else if (stMsg.msg == eTcpMsgSENDQ)
        {
            if (!processSEND_(pGamerContext, (uint8_t*)stMsg.msgRaw))
            {
                break;
            }
        }
        else if (    (stMsg.msg == eTcpMsgUPMOV)
                  || (stMsg.msg == eTcpMsgDOMOV)
                  || (stMsg.msg == eTcpMsgLEMOV)
                  || (stMsg.msg == eTcpMsgRIMOV)
                )
        {
            if (!processMOVE(pGamerContext, stMsg.msg, (uint8_t*)stMsg.msgRaw))
            {
                sendMsg(tcpSocket, eTcpMsgGOBYE);
                break;
            }
        }
    }

    removeGamePlayer(pGamerContext, pGamerContext->player);
    freePlayer(pGamerContext->player);

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
        log_error("GAMES<n>*** sending failure");
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
                    log_error("OGAME<m><s>*** for sending failure");
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
            log_error("NEWPL scan error!");
            return false;
        }
        gContext->player->portUDP = (uint16_t)port;

        printf("{%s} receive gamer ID {%s}\n", __FUNCTION__, gContext->player->id);

        // add this gamer to list of lstPlayers
        addGamePlayer(gContext, newGame, gContext->player); //ADD LAST
        
        if (!sendMsg(tcpSocket, eTcpMsgREGOK, (uint32_t)newGame->idGame))
        {
            log_error("REGOK sending failure!");
            return false;
        }
    }
    else // REGNO***
    {
        if (!sendMsg(tcpSocket, eTcpMsgREGNO))
        {
            log_error("REGNO sending failure!");
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
                        log_error("REGOK sending failure!");
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
                log_error("REGNO sending failure!");
                bRet = false;
            }
        }
    }
    else
    {
        log_error("REGIS scan error!");
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
            log_error("UNROK sending failure!");
            bRet = false;
        }
    }
    else
    {
        bRet = true;
        if (!sendMsg(tcpSocket, eTcpMsgDUNNO))
        {
            log_error("DUNNO sending failure!");
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
                    log_error("SIZE! sending failure!");
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
                log_error("DUNNO sending failure!");
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
                    log_error("LISTA sending failure!");
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
                            log_error("PLAYR sending failure!");
                            bRet = false;
                        }
                        gamerEl = gamerEl->next;
                    }
                }
            }
            pthread_mutex_unlock(&game->gameLock);   
            gameEl = gameEl->next;
        }
    }

    pthread_mutex_unlock(&gContext->serverCtx->serverLock);

    if (0 == isFound)
    {
        bRet = true;
        if (!sendMsg(tcpSocket, eTcpMsgDUNNO))
        {
            log_error("DUNNO sending failure!");
            bRet = false;
        }
    }
    return bRet;
}

//-> START*** -> start of game
bool processSTART(struct stGamerContext *gContext, uint8_t *bufer)
{
    int      tcpSocket = gContext->player->tcpSocket;
    bool     bRet      = true;
    
    if (gContext->player->pGame)
    {
        struct stGame *pGame = gContext->player->pGame;

        pthread_mutex_lock(&pGame->gameLock);
        gContext->player->isStarted = true;
        
        do
        {
            gContext->player->x = rand() % pGame->labirinth.width;
            gContext->player->y = rand() % pGame->labirinth.heigh;
        } 
        while (MAZE_WALL == MAZE_AT(pGame->labirinth.maze, (uint32_t)pGame->labirinth.width, (uint32_t)gContext->player->x, (uint32_t)gContext->player->y));

        pthread_mutex_unlock(&pGame->gameLock);

        if (isGameReadyToStart(gContext, pGame))
        {
            launchGame(gContext, pGame);
        }            
    }
    else
    {
        if (!sendMsg(tcpSocket, eTcpMsgNMEMB))
        {
            log_error("NMEMB sending failure!");
            bRet = false;
        }
    }        

    return bRet;
}

bool processGLIS(struct stGamerContext *gContext)
{
    int      tcpSocket = gContext->player->tcpSocket;
    bool     bRet      = false;
    
    if (gContext->player->pGame)
    {
        struct stGame *game = gContext->player->pGame;
        pthread_mutex_lock(&game->gameLock);
        if (game->isLaunched)
        {
            bRet = true;

            if (!sendMsg(tcpSocket, eTcpMsgGLISA, (uint32_t)game->lstPlayers->count))
            {
                log_error("eTcpMsgGLISA sending failure!");
                bRet = false;
            }
            if (bRet)
            {
                struct element_t *gamerEl = game->lstPlayers->first;
                while (gamerEl)
                {
                    struct stPlayer *gamer = (struct stPlayer *)(gamerEl->data);

                    if (!sendMsg(tcpSocket, 
                                 eTcpMsgGPLYR, 
                                 gamer->id, 
                                 (uint32_t)gamer->y, //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
                                 (uint32_t)gamer->x, 
                                 (uint32_t)gamer->score))
                    {
                        log_error("eTcpMsgGPLYR sending failure!");
                        bRet = false;
                    }
                    gamerEl = gamerEl->next;
                }
            }
        }

        pthread_mutex_unlock(&game->gameLock);
    }

    return bRet;
}

bool processMOVE(struct stGamerContext *gContext, enum msgId Id, uint8_t *bufer)
{
    uint32_t shift = 0;

    if (1 != scanMsg(bufer, Id, &shift))
    {
        log_error("eTcpMsgMOVE scan failure!");
        return false;
    }

    if (!gContext->player->pGame)
    {
        log_error("Game isn't statred!");
        return false;
    }

    bool ret = false;
    struct stGame *game = gContext->player->pGame;
    pthread_mutex_lock(&game->gameLock);
    if (game->isLaunched)
    {
        ret = true;
        uint16_t newX = gContext->player->x;
        uint16_t newY = gContext->player->y;
        uint16_t uScore = 1;
        uint16_t uGhost = 0;

        while (shift--)
        {
            if      (eTcpMsgDOMOV == Id) newY++;
            else if (eTcpMsgUPMOV == Id) newY--;
            else if (eTcpMsgLEMOV == Id) newX--;
            else if (eTcpMsgRIMOV == Id) newX++;
            if (MAZE_WALL == MAZE_AT(game->labirinth.maze, game->labirinth.width, newX, newY))
            {   
                if      (eTcpMsgDOMOV == Id) newY--;
                else if (eTcpMsgUPMOV == Id) newY++;
                else if (eTcpMsgLEMOV == Id) newX++;
                else if (eTcpMsgRIMOV == Id) newX--;
                break;
            }

            struct element_t* ghostEl = game->labirinth.ghosts->first;
            while (ghostEl)
            {
                struct stGhost *ghost = (struct stGhost*)(ghostEl->data);
                if ( (ghost->x == newX) && (ghost->y == newY))
                {
                    gContext->player->score += uScore;
                    uScore = uScore * 2;
                    uGhost++;

                    sendMsgTo(game->udpMctSocket,
                              (struct sockaddr*)&game->MctAddr,
                              sizeof(game->MctAddr),
                              eUdpMsgSCORE,
                              gContext->player->id, 
                              (uint32_t)gContext->player->score,
                              (uint32_t)ghost->y, //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
                              (uint32_t)ghost->x
                              );

                    struct element_t* ghostElNext = ghostEl->next;
                    free(ghost);
                    removeEl(game->labirinth.ghosts, ghostEl);
                    ghostEl = ghostElNext;
                    continue;
                }
                ghostEl = ghostEl->next;
            }
        }
        gContext->player->x = newX;
        gContext->player->y = newY;

        if (uGhost)
        {
            sendMsg(gContext->player->tcpSocket, 
                    eTcpMsgMOVEF, 
                    (uint32_t)gContext->player->y, //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
                    (uint32_t)gContext->player->x, 
                    (uint32_t)gContext->player->score);
        }
        else
        {
            sendMsg(gContext->player->tcpSocket, 
                    eTcpMsgMOVEA, 
                    (uint32_t)gContext->player->y, //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
                    (uint32_t)gContext->player->x 
                    );
        }
    }
    pthread_mutex_unlock(&game->gameLock);

    return ret;
}

bool processMALL_(struct stGamerContext *gContext, uint8_t *bufer)
{
    char pMsg[MAX_LEN];

    if (1 != scanMsg(bufer, eTcpMsgMALLQ, pMsg))
    {
        log_error("eTcpMsgMALLQ scan failure!");
        return false;
    }

    if (!gContext->player->pGame)
    {
        log_error("Game isn't statred!");
        return false;
    }

    bool ret = false;
    struct stGame *game = gContext->player->pGame;
    pthread_mutex_lock(&game->gameLock);

    if (game->isLaunched)
    {
        sendMsgTo(game->udpMctSocket, 
                  (struct sockaddr*)&game->MctAddr, 
                  sizeof(game->MctAddr), 
                  eUdpMsgMESSA, 
                  gContext->player->id, 
                  pMsg);

        sendMsg(gContext->player->tcpSocket, eTcpMsgMALLA);
        ret = true;
    }

    pthread_mutex_unlock(&game->gameLock);

    return ret;
}


bool processSEND_(struct stGamerContext *gContext, uint8_t *bufer)
{
    char pMsg[MAX_LEN];
    char pPlayerId[USER_ID_LEN+1];

    if (2 != scanMsg(bufer, eTcpMsgSENDQ, pPlayerId, pMsg))
    {
        log_error("eTcpMsgSENDQ scan failure!");
        return false;
    }

    if (!gContext->player->pGame)
    {
        log_error("Game isn't statred!");
        return false;
    }

    bool ret = false;
    struct stGame *game = gContext->player->pGame;
    pthread_mutex_lock(&game->gameLock);

    if (game->isLaunched)
    {
        struct element_t *gamerEl = game->lstPlayers->first;
        struct stPlayer  *gamer   = NULL;
        while (gamerEl)
        {
            gamer = (struct stPlayer *)(gamerEl->data);

            if (0 == memcmp(gamer->id, pPlayerId, USER_ID_LEN))
            {
                ret = true;
                break;
            }
            gamerEl = gamerEl->next;
        }

        if (ret)
        {
            struct sockaddr_in udpAddr;
            udpAddr.sin_family      = AF_INET;
            udpAddr.sin_addr.s_addr = htonl(gamer->ipAddress);
            udpAddr.sin_port        = htons(gamer->portUDP);
            
            sendMsgTo(gContext->player->pGame->udpMsgSocket, 
                      (struct sockaddr*)&udpAddr, 
                      sizeof(udpAddr), 
                      eUdpMsgMESSP, 
                      gContext->player->id, 
                      pMsg);

            sendMsg(gContext->player->tcpSocket, eTcpMsgSENDA);
        }
        else
        {
            sendMsg(gContext->player->tcpSocket, eTcpMsgNSEND);
        }
    }

    pthread_mutex_unlock(&game->gameLock);

    return ret;
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
        log_fatal("malloc stGame");
        return NULL;
    }
    memset(game, 0, sizeof(struct stGame));
    game->isLaunched = 0; // is done abouve
    game->thread = 0;
    pthread_mutexattr_t Attr;
    pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&game->gameLock, &Attr);

    pthread_mutex_lock(&gContext->serverCtx->serverLock);
    game->idGame = ++gContext->serverCtx->lastGameId;
    pthread_mutex_unlock(&gContext->serverCtx->serverLock);

    //create list of lstPlayers
    game->lstPlayers = (struct listElements_t *)malloc(sizeof(struct listElements_t));
    if (!game->lstPlayers)
    {
        log_fatal("malloc lstPlayers in createGame");
        return NULL;
    }
    memset(game->lstPlayers, 0, sizeof(struct listElements_t));

    //create labirinth
    game->labirinth.width = 10;//(rand() % 96) + 32;
    game->labirinth.heigh = 10;//(rand() % 96) + 32;
    game->labirinth.maze  = maze_create(game->labirinth.width, game->labirinth.heigh);
    game->udpMsgSocket    = socket(AF_INET, SOCK_DGRAM, 0);
    game->udpMctSocket    = socket(AF_INET, SOCK_DGRAM, 0);
    
    game->MctAddr.sin_family      = AF_INET;
    game->MctAddr.sin_addr.s_addr = inet_addr(MULTI_CAST_ADDR);
    game->MctAddr.sin_port        = htons(9876 + game->idGame);

    //create list of ghosts and pointer to stGrid
    game->labirinth.ghosts = (struct listElements_t *)malloc(sizeof(struct listElements_t));
    if (!game->labirinth.ghosts)
    {
        log_fatal("malloc list of ghosts or grid in createGame");
        return NULL;
    }
    memset(game->labirinth.ghosts, 0, sizeof(struct listElements_t));

    uint8_t bGhostsCount = 16;//(rand() % ((game->labirinth.width / 2) % 255)) + 8;

    while (bGhostsCount--)
    {
        struct stGhost *pGhost = (struct stGhost *)malloc(sizeof(struct stGhost));
        memset(pGhost, 0, sizeof(sizeof(struct stGhost)));
        pGhost->id = bGhostsCount;
        do {
            pGhost->x = rand() % game->labirinth.width;
            pGhost->y = rand() % game->labirinth.heigh;
        } while (MAZE_WALL == MAZE_AT(game->labirinth.maze, (uint32_t)game->labirinth.width, (uint32_t)pGhost->x, (uint32_t)pGhost->y));

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

        CLOSE(pGame->udpMsgSocket);
        CLOSE(pGame->udpMctSocket);

        log_info("Free game #%d players", pGame->idGame);
        // free lstPlayers
        while (pGame->lstPlayers->first)
        {
            //REMARK: player context belongs to communication thread and will be released in there
            ((struct stPlayer*)(pGame->lstPlayers->first->data))->pGame = NULL;
            removeEl(pGame->lstPlayers, pGame->lstPlayers->first);
        }
        FREE_MEM(pGame->lstPlayers);

        log_info("Free game #%d ghosts", pGame->idGame);
        while (pGame->labirinth.ghosts->first)
        {
            struct stGhost *ghost = (struct stGhost *)(pGame->labirinth.ghosts->first->data);
            FREE_MEM(ghost);
            removeEl(pGame->labirinth.ghosts, pGame->labirinth.ghosts->first);
        }

        FREE_MEM(pGame->labirinth.ghosts);
        FREE_MEM(pGame->labirinth.maze);

        bool bRunning = pGame->isLaunched;

        pGame->isLaunched = false;

        // free game
        pthread_mutex_unlock(&pGame->gameLock);

        if (bRunning)
        {
            log_info("Wait for game #%d thread", pGame->idGame);
            pthread_join(pGame->thread, NULL);
        }

        pthread_mutex_destroy(&pGame->gameLock);
        log_info("Release game #%d memory", pGame->idGame);
        FREE_MEM(pGame);
    }
}

// create new gamer
struct stPlayer *createPlayer(int32_t socket, uint32_t uIpv4)
{
    struct stPlayer *newPlayer = (struct stPlayer *)malloc(sizeof(struct stPlayer));
    if (!newPlayer)
    {
        log_fatal("malloc gamer failure");
        return NULL;
    }

    memset(newPlayer, 0, sizeof(struct stPlayer));

    newPlayer->tcpSocket  = socket;
    newPlayer->ipAddress   = uIpv4; //TODO check that need htons
    newPlayer->pGame       = NULL;
    newPlayer->isStarted   = false;

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
        log_error("removeGamePlayer() args error!");
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
            log_info("removeGamePlayer(%s)!", ((struct stPlayer *)pIter->data)->id);
            removeEl(player->pGame->lstPlayers, pIter);
            player->pGame = NULL;
            bFound = true;
            break;
        }
        pIter = pIter->next;
    }

    size_t szPlayersCount = pCurrentGame->lstPlayers->count;
    pthread_mutex_unlock(&pCurrentGame->gameLock);

    if (isGameReadyToStart(gContext, pCurrentGame))
    {
        launchGame(gContext, pCurrentGame);
    }            
    else if (  (!szPlayersCount) && (bFound) )
    {
        if (removeGame(gContext, pCurrentGame, false))
        {
            freeGame(pCurrentGame);
        }
    }

    if (!bFound)
    {
        log_error("removeGamePlayer() failed, gamer not found!");
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
        log_error("removeGame() args error!");
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

bool isGameReadyToStart(struct stGamerContext *gContext, struct stGame *game)
{
    pthread_mutex_lock(&game->gameLock);
    // check if Start was recieved from all gamers
    bool isReadyToStart = true;
    size_t szCount = game->lstPlayers->count;
    struct element_t *playerEl  = game->lstPlayers->first;
    while (playerEl)
    {
        struct stPlayer *player = (struct stPlayer *)(playerEl->data);
        if (!player->isStarted)
        {
            isReadyToStart = false;
            break;
        }     
        playerEl = playerEl->next;           
    }
    pthread_mutex_unlock(&game->gameLock);

    return ((szCount > 0) && (isReadyToStart));
}

void printPlayers(struct stGamerContext *gContext, const char *pCaller)
{
    if (!gContext)
    {
        log_error("printPlayers() args error!");
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