#include "engine.h"

struct stGhostPos
{
    uint16_t x;
    uint16_t y;
};


void *routine_game(void *args)
{
    struct stGame *game     = (struct stGame *)args;
    bool           isExit   = false;
    const uint32_t sleepMs = 10;
    uint32_t       iter    = 0;
    uint32_t       iterMod = GHOST_MOVEMENT_INTERVAL_MS / sleepMs;


    usleep(1000 * 500); //initial sleep to let players to connect to multicast sockets, etc. 

    pthread_mutex_lock(&game->gameLock);
    log_info("Create game %u thread", (uint32_t)game->idGame);
    sendMsgTo(game->udpMctSocket, 
              (struct sockaddr*)&game->MctAddr, 
              sizeof(game->MctAddr), 
              eUdpMsgMESSA, 
              "_SERVER_", 
              "Welcome to game, the goal of the game is to catch all ghosts.\nGood luck!");
    pthread_mutex_unlock(&game->gameLock);

    while (false == isExit)
    {
        pthread_mutex_lock(&game->gameLock);
        if ((!game->isLaunched) || (!game->labirinth.ghosts) || (!game->labirinth.ghosts->count))
        {
            isExit = true;
        }

        if ((!isExit) && (0 == (iter % iterMod)))
        {
            struct element_t *ghostEl = game->labirinth.ghosts->first;
            while (ghostEl)
            {
                struct stGhost* ghost = (struct stGhost*)ghostEl->data;

                struct stGhostPos pPos[4]; //4 possible direction
                size_t szPos = 0;

                if (ghost->x > 0)
                {
                    if (MAZE_WALL != MAZE_AT(game->labirinth.maze, game->labirinth.width, ghost->x-1, ghost->y))
                    {
                        pPos[szPos].x = ghost->x-1;
                        pPos[szPos].y = ghost->y;
                        szPos++;
                    }
                }

                if (ghost->x + 1 < game->labirinth.width)
                {
                    if (MAZE_WALL != MAZE_AT(game->labirinth.maze, game->labirinth.width, ghost->x+1, ghost->y))
                    {
                        pPos[szPos].x = ghost->x+1;
                        pPos[szPos].y = ghost->y;
                        szPos++;
                    }
                }

                if (ghost->y > 0)
                {
                    if (MAZE_WALL != MAZE_AT(game->labirinth.maze, game->labirinth.width, ghost->x, ghost->y-1))
                    {
                        pPos[szPos].x = ghost->x;
                        pPos[szPos].y = ghost->y-1;
                        szPos++;
                    }
                }

                if (ghost->y + 1 < game->labirinth.heigh)
                {
                    if (MAZE_WALL != MAZE_AT(game->labirinth.maze, game->labirinth.width, ghost->x, ghost->y+1))
                    {
                        pPos[szPos].x = ghost->x;
                        pPos[szPos].y = ghost->y+1;
                        szPos++;
                    }
                }

                if (szPos)
                {
                    uint32_t szNewPos = rand() % szPos;
                    ghost->x = pPos[szNewPos].x;
                    ghost->y = pPos[szNewPos].y;
                }

                sendMsgTo(game->udpMctSocket, 
                         (struct sockaddr*)&game->MctAddr, 
                         sizeof(game->MctAddr), 
                         eUdpMsgGHOST, 
                         (uint32_t)ghost->y, //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
                         (uint32_t)ghost->x 
                         );

                ghostEl = ghostEl->next;
            }
        }

        pthread_mutex_unlock(&game->gameLock);

        usleep(1000 * sleepMs);
        iter ++;
    }

    pthread_mutex_lock(&game->gameLock);
    if (game->lstPlayers && game->lstPlayers->count)
    {
        struct element_t *gamerEl = game->lstPlayers->first;
        struct stPlayer  *Winner   = NULL;
        while (gamerEl)
        {
            struct stPlayer *gamer = (struct stPlayer *)(gamerEl->data);

            if ((!Winner) || (gamer->score > Winner->score))
            {
                Winner = gamer;
            }
            gamerEl = gamerEl->next;
        }

        sendMsgTo(game->udpMctSocket, 
                  (struct sockaddr*)&game->MctAddr, 
                  sizeof(game->MctAddr), 
                  eUdpMsgENDGA, 
                  Winner->id, 
                  (uint32_t)Winner->score);
    }

    log_info("Stop game %u thread", (uint32_t)game->idGame);
    pthread_mutex_unlock(&game->gameLock);

    return NULL;
}


int launchGame(struct stGamerContext *gContext, struct stGame *game)
{
    int rez = EXIT_SUCCESS;

    pthread_mutex_lock(&game->gameLock);
    bool startThread = false;
    if (!game->isLaunched)
    {
        struct element_t *playerEl = game->lstPlayers->first;
        while (playerEl)
        {
            struct stPlayer *player = (struct stPlayer *)(playerEl->data);
            
            sendMsg(player->tcpSocket, eTcpMsgWELCO, 
                    (uint32_t)game->idGame,
                    (uint32_t)game->labirinth.heigh,
                    (uint32_t)game->labirinth.width,
                    (uint32_t)game->labirinth.ghosts->count,
                    ntohl((uint32_t)inet_addr (MULTI_CAST_ADDR)),
                    (uint32_t)ntohs(game->MctAddr.sin_port)
                    );

            printf("Player %s, %u, %u\n",
                   player->id,
                   (uint32_t)player->x,
                   (uint32_t)player->y 
                  );

            sendMsg(player->tcpSocket, eTcpMsgPOSIT, 
                    player->id,
                    (uint32_t)player->y, //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
                    (uint32_t)player->x
                    );

            playerEl = playerEl->next;
        }

        game->isLaunched = true;
        startThread      = true;
    }
    pthread_mutex_unlock(&game->gameLock);

    if (startThread)
    {
        pthread_t th;
        int phRez = pthread_create(&th, NULL, routine_game, (void *)game);
        if (phRez != 0) // error
        {
            log_error("pthread_create failure"); 
            rez = EXIT_FAILURE;
            goto lExit;
        }
        else
        {
            pthread_mutex_lock(&game->gameLock);
            game->thread = th;
            pthread_mutex_unlock(&game->gameLock);
        }
    }
    
lExit:    
    return rez;
}

