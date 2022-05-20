#include "common.h"

void freeGames(struct stContext *pCtx)
{
    if ((!pCtx) || (!pCtx->pGames)) return;

    for (uint8_t i = 0; i < pCtx->gamesCount; i++)
    {
        free(pCtx->pGames[i].pPlayers);
    }
    free(pCtx->pGames);
    pCtx->pGames = NULL;
    pCtx->gamesCount = 0;
    pCtx->pSelectedGame = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///   Functions for cheat mode                                                                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// add position to cheat path
void cheatPathAdd(struct stContext *pCtx, uint32_t X, uint32_t Y)
{
    struct stCheatCoord *new = (struct stCheatCoord*)malloc(sizeof(struct stCheatCoord));
    new->uX = X;
    new->uY = Y;
    pushLast(&pCtx->stGameSet.listCheatPath, new);
}

// take last one from list
const struct stCheatCoord* cheatPathGetLast(struct stContext *pCtx)
{
    if (pCtx->stGameSet.listCheatPath.last)
    {
        return (struct stCheatCoord *)(pCtx->stGameSet.listCheatPath.last->data);
    }
    return NULL;
}

void cheatPathDelLast(struct stContext *pCtx)
{
    struct stCheatCoord *pCoord = (struct stCheatCoord *)pullLast(&pCtx->stGameSet.listCheatPath);
    FREE(pCoord);
}

// free path
void cheatPathClear(struct stContext *pCtx)
{
    while (pCtx->stGameSet.listCheatPath.first)
    {
        free(pCtx->stGameSet.listCheatPath.first->data);
        removeEl(&pCtx->stGameSet.listCheatPath, pCtx->stGameSet.listCheatPath.first);
    }
}

// main function
void cheatGameSession(struct stContext *pCtx)
{
    uint32_t moves = 0;
    do
    {
        bool     bRealMove = true;
        uint32_t newX     = pCtx->stGameSet.myX;
        uint32_t newY     = pCtx->stGameSet.myY;
        moves++;

        if ((pCtx->stGameSet.myY) && (eMazeUnknown == MAZE_AT(pCtx->stGameSet.pMazeCheat, 
                                                               pCtx->stGameSet.mazeW, 
                                                               pCtx->stGameSet.myX, 
                                                               pCtx->stGameSet.myY-1))
            )
        {
            newY --;
            sendMsg(pCtx->tcpSocket, eTcpMsgUPMOV, 1);
        }
        else if (((pCtx->stGameSet.myY + 1) < pCtx->stGameSet.mazeH) && (eMazeUnknown == MAZE_AT(pCtx->stGameSet.pMazeCheat, 
                                                                                                   pCtx->stGameSet.mazeW, 
                                                                                                   pCtx->stGameSet.myX, 
                                                                                                   pCtx->stGameSet.myY+1))
            )
        {
            newY ++;
            sendMsg(pCtx->tcpSocket, eTcpMsgDOMOV, 1);
        }
        else if (((pCtx->stGameSet.myX + 1) < pCtx->stGameSet.mazeW) && (eMazeUnknown == MAZE_AT(pCtx->stGameSet.pMazeCheat, 
                                                                                                   pCtx->stGameSet.mazeW, 
                                                                                                   pCtx->stGameSet.myX+1, 
                                                                                                   pCtx->stGameSet.myY))
            )
        {
            newX++;
            sendMsg(pCtx->tcpSocket, eTcpMsgRIMOV, 1);
        }
        else if (pCtx->stGameSet.myX && (eMazeUnknown == MAZE_AT(pCtx->stGameSet.pMazeCheat, 
                                                                  pCtx->stGameSet.mazeW, 
                                                                  pCtx->stGameSet.myX-1, 
                                                                  pCtx->stGameSet.myY))
            )
        {
            newX--;
            sendMsg(pCtx->tcpSocket, eTcpMsgLEMOV, 1);
        }
        else
        {
            const struct stCheatCoord *pLast = cheatPathGetLast(pCtx);
            if (pLast)
            {
                if (pCtx->stGameSet.myX < pLast->uX)
                {
                    sendMsg(pCtx->tcpSocket, eTcpMsgRIMOV, 1);
                }
                else if (pCtx->stGameSet.myX > pLast->uX)
                {
                    sendMsg(pCtx->tcpSocket, eTcpMsgLEMOV, 1);
                }
                else if (pCtx->stGameSet.myY > pLast->uY)
                {
                    sendMsg(pCtx->tcpSocket, eTcpMsgUPMOV, 1);
                }
                else if (pCtx->stGameSet.myY < pLast->uY)
                {
                    sendMsg(pCtx->tcpSocket, eTcpMsgDOMOV, 1);
                }

                newX = pLast->uX;
                newY = pLast->uY;

                cheatPathDelLast(pCtx);
                bRealMove = false;
            }
        }

        struct stRawMessage stRawMsg;

        if (socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stRawMsg))
        {
            uint32_t x, y;
            if (stRawMsg.eMsg == eTcpMsgMOVEA)
            {
                scanMsg(stRawMsg.pMsgRaw, eTcpMsgMOVEA, &y, &x); //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
            }
            else if (stRawMsg.eMsg == eTcpMsgMOVEF)
            {
                scanMsg(stRawMsg.pMsgRaw, eTcpMsgMOVEF, &y, &x, &pCtx->stGameSet.score); //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
            }

            if ((pCtx->stGameSet.myX == x) && (pCtx->stGameSet.myY == y))
            {
                MAZE_AT(pCtx->stGameSet.pMazeCheat, pCtx->stGameSet.mazeW, newX, newY) = eMazeWall;
                MAZE_AT(pCtx->stGameSet.pMaze, pCtx->stGameSet.mazeW, newX, newY)      = eMazeWall;
            }
            else 
            {
                MAZE_AT(pCtx->stGameSet.pMazeCheat, pCtx->stGameSet.mazeW, x, y) = eMazeEmpty;
                MAZE_AT(pCtx->stGameSet.pMaze, pCtx->stGameSet.mazeW, x, y)      = eMazeEmpty;

                if (bRealMove)
                {
                    cheatPathAdd(pCtx, pCtx->stGameSet.myX, pCtx->stGameSet.myY);
                }

                pCtx->stGameSet.myX = x;
                pCtx->stGameSet.myY = y;
            }
        }
    } while (moves < pCtx->stGameSet.cheatSpeed);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void freeGameSet(struct stContext *pCtx)
{
    if (!pCtx)  return;

    CLOSE(pCtx->stGameSet.udpMctSocket);
    FREE(pCtx->stGameSet.pPlayers);
    FREE(pCtx->stGameSet.pFantoms);
    FREE(pCtx->stGameSet.pMaze);
    FREE(pCtx->stGameSet.pMazeCheat);

    while (pCtx->stGameSet.stListMsg.first)
    {
        free(pCtx->stGameSet.stListMsg.first->data);
        removeEl(&pCtx->stGameSet.stListMsg, pCtx->stGameSet.stListMsg.first);
    }

    cheatPathClear(pCtx);

    pCtx->stGameSet.stUdpBuf.szWritten = 0;
    pCtx->stGameSet.cId                = 0;
    pCtx->stGameSet.countPlayers       = 0;
    pCtx->stGameSet.fantoms            = 0;
    pCtx->stGameSet.mazeW              = 0;
    pCtx->stGameSet.mazeH              = 0;
    pCtx->stGameSet.pMaze              = 0;
    pCtx->stGameSet.myX                = 0;
    pCtx->stGameSet.myY                = 0;
    pCtx->stGameSet.score              = 0;
}

bool updateGamesInfo(struct stContext *pCtx, bool bSendRequest) //OK
{
    if (bSendRequest)
    {
        sendMsg(pCtx->tcpSocket, eTcpMsgGAMEQ);
    }

    struct stRawMessage stMsg;
    if (!socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stMsg))
    {
        log_error("eTcpMsgGAMES message timeout!");
        return false;
    }

    if (stMsg.eMsg != eTcpMsgGAMES)
    {
        log_error("%d!= eTcpMsgGAMES not expected message", stMsg.eMsg);
        return false;
    }

    freeGames(pCtx);

    pCtx->gamesCount = *(uint8_t*)(stMsg.pMsgRaw +6);

    if (!pCtx->gamesCount)
    {
        return true;
    }

    pCtx->pGames = (struct stGameInfo *)malloc(sizeof(struct stGameInfo) * pCtx->gamesCount);
    if (!pCtx->pGames)
    {
        log_fatal(" Memory Allocation");
        return false;
    }

    memset(pCtx->pGames, 0, sizeof(struct stGameInfo) * pCtx->gamesCount);

    for (uint8_t j = 0; j < pCtx->gamesCount; j++)
    {
        if (!socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stMsg))
        {
            log_error("message timeout");
            return false;
        }

        if (stMsg.eMsg != eTcpMsgOGAME)
        {
            log_error("%d != eTcpMsgOGAME", stMsg.eMsg);
            return false;
        }

        pCtx->pGames[j].cId      = *(uint8_t*)(stMsg.pMsgRaw +6);
        pCtx->pGames[j].countPlayers = *(uint8_t*)(stMsg.pMsgRaw +8);

    }

    for (uint8_t i = 0; i < pCtx->gamesCount; i++)
    {
        if (!sendMsg(pCtx->tcpSocket, eTcpMsgSIZEQ, (uint32_t)pCtx->pGames[i].cId))
        {
            log_error("can't send message sendSizeQuery");
            return false;
        }

        if (!socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stMsg))
        {
            log_error("message timeout");
            return false;
        }

        if (stMsg.eMsg != eTcpMsgSIZEA)
        {
            log_error("%d != eTcpMsgSIZEA", stMsg.eMsg);
            return false;
        }

        uint32_t uM, uH, uW;
        scanMsg(stMsg.pMsgRaw, eTcpMsgSIZEA, &uM, &uH, &uW);

        if (uM != pCtx->pGames[i].cId)
        {
            log_error("%u != %u unexpected game ID", uM, pCtx->pGames[i].cId);
            return false;
        }

        pCtx->pGames[i].heigth = (uint16_t)uH;
        pCtx->pGames[i].width = (uint16_t)uW;
    }

    return true;
}

void printGamesInfo(struct stContext *pCtx)
{
    printf("Available games: \n");
    if (!pCtx->gamesCount)
    {
        printf("No games available!\n");
        return;
    }
    for (uint8_t i = 0; i < pCtx->gamesCount; i++)
    {
        printf(" * ID: %u, Players: %u, size: %ux%u\n", 
               (uint32_t)pCtx->pGames[i].cId,
               (uint32_t)pCtx->pGames[i].countPlayers,
               (uint32_t)pCtx->pGames[i].width,
               (uint32_t)pCtx->pGames[i].heigth
               );
    }
    printf("--------------------------\n\n");
}

bool startGamesMenu(struct stContext *pCtx)
{
    while (!pCtx->bExit)
    {
        printf("=MENU=====================\n");
        printf("1 -> Print games list\n");
        printf("2 -> Update games list\n");
        printf("3 -> Create new game\n");
        printf("4 -> Register in game\n");
        printf("0 -> Exit\n");
        printf("==========================\n");
        printf("Enter your choise: ");
        fflush(stdout);

        uint32_t ch = read_input_uint();

        if (0 == ch)
        {
            printf("Exit ...\n");
            break;
        }
        else if (1 == ch)
        {
            printGamesInfo(pCtx);
        }
        else if (2 == ch)
        {
            updateGamesInfo(pCtx, true);
            printGamesInfo(pCtx);
        }
        else if ((3 == ch) || (4 == ch))
        {
            if (3 == ch)
            {
                sendMsg(pCtx->tcpSocket, eTcpMsgNEWPL, pCtx->userId, (uint32_t)pCtx->udpPeerPort);
            }
            else
            {
                printf("Please enter game ID: "); fflush(stdout);
                uint32_t gameId = read_input_uint();
                sendMsg(pCtx->tcpSocket, eTcpMsgREGIS, pCtx->userId, (uint32_t)pCtx->udpPeerPort, gameId);
            }

            pCtx->pSelectedGame = NULL;
            struct stRawMessage stMsg;
            if (socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stMsg))
            {
                if (stMsg.eMsg == eTcpMsgREGOK)
                {
                    uint32_t m = 0;
                    scanMsg(stMsg.pMsgRaw, eTcpMsgREGOK, &m);

                    updateGamesInfo(pCtx, true);

                    for (uint8_t i = 0; i < pCtx->gamesCount; i++)
                    {
                        if (pCtx->pGames[i].cId == m)
                        {
                            pCtx->pSelectedGame = &pCtx->pGames[i];
                            break;
                        }
                    }

                    startGame(pCtx);
                }
                else 
                {
                    printf("ERROR: Can't register to game\n");
                }
            }
        }
    }
}

bool updatePlayers(struct stContext *pCtx)
{
    if (!pCtx->pSelectedGame)
    {
        log_error("Current game isn't selected!");
        return false;
    }
    
    sendMsg(pCtx->tcpSocket, eTcpMsgLISTQ, (uint32_t)pCtx->pSelectedGame->cId);

    struct stRawMessage stMsg;
    if (!socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stMsg))
    {
        log_error("eTcpMsgLISTA message timeout!");
        return false;
    }

    if (stMsg.eMsg == eTcpMsgDUNNO)
    {
        char pTest[256];
        sprintf(pTest, "Session %u isn't recognized by server", (uint32_t)pCtx->pSelectedGame->cId);
        return false;
    }
    else if (stMsg.eMsg != eTcpMsgLISTA)
    {
        log_error("%d!= eTcpMsgLISTA not expected message", stMsg.eMsg);
        return false;
    }

    free(pCtx->pSelectedGame->pPlayers);
    pCtx->pSelectedGame->pPlayers = NULL;
    pCtx->pSelectedGame->countPlayers = 0;

    uint32_t m, s;
    if (2 != scanMsg(stMsg.pMsgRaw, eTcpMsgLISTA, &m, &s))
    {
        log_error("Can't parse eTcpMsgLISTA");
        return false;
    }
    
    pCtx->pSelectedGame->countPlayers = (uint8_t)s;

    if (!pCtx->pSelectedGame->countPlayers)
    {
        return true;
    }

    pCtx->pSelectedGame->pPlayers = (struct stPlId*)malloc(sizeof(struct stPlId) * (size_t)pCtx->pSelectedGame->countPlayers);
    if (!pCtx->pSelectedGame->pPlayers)
    {
        log_fatal(" Memory Allocation");
        return false;
    }

    memset(pCtx->pSelectedGame->pPlayers, 0, sizeof(struct stPlId) * (size_t)pCtx->pSelectedGame->countPlayers);

    for (uint8_t i = 0; i < pCtx->pSelectedGame->countPlayers; i++)
    {
        if (!socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stMsg))
        {
            log_error("message timeout");
            return false;
        }

        if (    (stMsg.eMsg != eTcpMsgPLAYR)
             || (1 != scanMsg(stMsg.pMsgRaw, eTcpMsgPLAYR, pCtx->pSelectedGame->pPlayers[i].pId))
           )  
        {
            log_error("%d != eTcpMsgPLAYR", stMsg.eMsg);
            return false;
        }
    }

    return true;
}

void printPlayers(struct stContext *pCtx)
{
    printf("Active players: \n");
    if (!pCtx->pSelectedGame->countPlayers)
    {
        printf("No players!\n");
        return;
    }
    for (uint8_t i = 0; i < pCtx->pSelectedGame->countPlayers; i++)
    {
        printf(" * ID: %s\n", pCtx->pSelectedGame->pPlayers[i].pId);
    }
    printf("--------------------------\n\n");
}


void startGame(struct stContext *pCtx)
{
    while (!pCtx->bExit)
    {
        printf("=MENU=====================\n");
        printf("1 -> Update players list\n");
        printf("2 -> Start game\n");
        printf("0 -> Unreg game\n");
        printf("==========================\n");
        printf("Enter your choise: ");
        fflush(stdout);

        uint32_t ch = read_input_uint();

        if (0 == ch)
        {
            struct stRawMessage stMsg;
            sendMsg(pCtx->tcpSocket, eTcpMsgUNREG);
            if (!socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stMsg))
            {
                log_error("message timeout!");
            }
            else if ((stMsg.eMsg != eTcpMsgUNROK) && (stMsg.eMsg != eTcpMsgDUNNO))
            {
                log_error("Unknow message %d!", stMsg.eMsg);
            }
            break;
        }
        else if (1 == ch)
        {
            updatePlayers(pCtx);
            printPlayers(pCtx);
        }
        else if (2 == ch)
        {
            sendMsg(pCtx->tcpSocket, eTcpMsgSTART);
            printf("Starting game: waiting for others ... \n");

            struct stRawMessage stMsg;
            stMsg.eMsg = eUdpMsgMaxValue;

            uint32_t stab = 0, f = 0, mctIp = 0, mctPort = 0, x = 0, y = 0;
            bool bSuccess = socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stMsg);

            if ((bSuccess) && (stMsg.eMsg == eTcpMsgWELCO))
            {
                bSuccess = (6 == scanMsg(stMsg.pMsgRaw, eTcpMsgWELCO, &stab, &stab, &stab, &f, &mctIp, &mctPort));
            }

            if (bSuccess)
            {
                bSuccess = socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stMsg);
            }

            if ((bSuccess) && (stMsg.eMsg == eTcpMsgPOSIT))
            {
                char pId[USER_ID_LEN+1];
                bSuccess = (3 == scanMsg(stMsg.pMsgRaw, eTcpMsgPOSIT, pId, &y, &x)); //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
            }

            if (bSuccess)
            {
                struct stGame *pGame = &pCtx->stGameSet;
                bool           bRet  = true;

                pGame->stUdpBuf.szWritten = 0;
                pGame->myX               = x;
                pGame->myY               = y;
                pGame->myNextMoveX       = x;
                pGame->myNextMoveY       = y;
                pGame->bCheat             = false;
                pGame->pMazeCheat         = NULL;
                pGame->cheatSpeed        = 3;

                printf("\nCreate Game set, Fantoms %u, Multicast Ip:%u.%u.%u.%u:%u my coord %u:%u\n", 
                        f,  
                        (uint32_t)((mctIp >> 24) & 0xFF),
                        (uint32_t)((mctIp >> 16) & 0xFF),
                        (uint32_t)((mctIp >> 8) & 0xFF),
                        (uint32_t)(mctIp & 0xFF),
                        (uint32_t)mctPort,
                        x, y);

                pGame->cId        = pCtx->pSelectedGame->cId;
                pGame->score     = 0;
                pGame->gameCycle = 0;

                pGame->udpMctSocket = udp_make_multicast_socket(mctIp, mctPort);
                
                if (pGame->udpMctSocket < 0)
                {
                    log_error("Can't create multicast socket %u %u", mctIp, mctPort);
                    bSuccess = false;
                }

                memset(pGame->pFantoms, 0, pGame->fantomsMax * sizeof(struct stFantom));

                pGame->mazeW = pCtx->pSelectedGame->width;
                pGame->mazeH = pCtx->pSelectedGame->heigth;
                pGame->pMaze  = (enum eMaze*)malloc(pGame->mazeW * pGame->mazeH * sizeof(enum eMaze));
                if (!pGame->pMaze)
                {
                    log_fatal("Can't allocate memory");
                    bSuccess = false;
                }

                for (uint32_t i = 0; i < pGame->mazeW * pGame->mazeH; i++)
                {
                    pGame->pMaze[i] = eMazeUnknown;
                }

                updatePlayersEx(pCtx);

                pGame->fantoms    = 0;
                pGame->fantomsMax = f;
                pGame->pFantoms = (struct stFantom*)malloc(pGame->fantomsMax * sizeof(struct stFantom));
                if (!pGame->pFantoms)
                {
                    log_error("Can't allocate memory");
                    bSuccess = false;
                }

                MAZE_AT(pGame->pMaze, pGame->mazeW, pGame->myX, pGame->myY) = eMazeEmpty;
            }

            if (bSuccess)
            {
                play(pCtx);
            }
            else
            {
                log_error("game creation failed!");
            }
        }
    }
}


void updatePlayersEx(struct stContext *pCtx)
{
    struct stRawMessage stRawMsg;

    sendMsg(pCtx->tcpSocket, eTcpMsgGLISQ);
    if (socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stRawMsg))
    {
        uint32_t uPlayers = 0;
        if (stRawMsg.eMsg == eTcpMsgGLISA)
        {
            if (1 == scanMsg(stRawMsg.pMsgRaw, eTcpMsgGLISA, &uPlayers))
            {
                pCtx->stGameSet.countPlayers = (uint32_t)uPlayers;
                FREE(pCtx->stGameSet.pPlayers);
                pCtx->stGameSet.pPlayers = (struct stPlayer*)malloc(uPlayers * sizeof(struct stPlayer));

                memset(pCtx->stGameSet.pPlayers, 0, pCtx->stGameSet.countPlayers * sizeof(struct stPlayer));
                for (uint32_t i = 0; i < uPlayers; i++)
                {
                    if (socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stRawMsg))
                    {
                        if (4 == scanMsg(stRawMsg.pMsgRaw, 
                                         eTcpMsgGPLYR, 
                                         &pCtx->stGameSet.pPlayers[i].plId,
                                         &pCtx->stGameSet.pPlayers[i].plY, //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
                                         &pCtx->stGameSet.pPlayers[i].plX,
                                         &pCtx->stGameSet.pPlayers[i].score
                                        )
                        )
                        {
                            MAZE_AT(pCtx->stGameSet.pMaze, pCtx->stGameSet.mazeW, 
                                    pCtx->stGameSet.pPlayers[i].plX, pCtx->stGameSet.pPlayers[i].plY) = eMazeEmpty;
                        }
                    }
                    else 
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            log_error("unknown message");
        }
    }
}

void printPlayersEx(struct stContext *pCtx)
{
    printf("Game players: \n");
    if (!pCtx->stGameSet.countPlayers)
    {
        printf("No players!\n");
        return;
    }

    for (uint32_t i = 0; i < pCtx->stGameSet.countPlayers; i++)
    {
        printf(" #%d [%s] at %ux%u, score %u\n",
               i,
               pCtx->stGameSet.pPlayers[i].plId,
               pCtx->stGameSet.pPlayers[i].plX,
               pCtx->stGameSet.pPlayers[i].plY,
               pCtx->stGameSet.pPlayers[i].score
              );
    }
    printf("--------------------------\n\n");
}


void receivePersonalMessages(struct stContext *pCtx)
{
    struct stRawMessage stRawMsg;
    while (socketReadMsgEx(pCtx->udpPeerSocket, &pCtx->stUdpBuf, &stRawMsg, 10))
    {
        if (stRawMsg.eMsg == eUdpMsgMESSP)
        {
            struct stMsg *pMsg = (struct stMsg*)malloc(sizeof(struct stMsg));
            if (pMsg)
            {
                if (2 == scanMsg(stRawMsg.pMsgRaw, eUdpMsgMESSA, pMsg->pId, pMsg->pMsg))
                {
                    pushLast(&pCtx->stGameSet.stListMsg, pMsg);
                }
                else
                {
                    free(pMsg);
                }
            }
        }
    }
}

void receiveMultiCastMessages(struct stContext *pCtx)
{
    struct stRawMessage stRawMsg;
    uint32_t fantomIndex = 0;
    while (socketReadMsgEx(pCtx->stGameSet.udpMctSocket, &pCtx->stGameSet.stUdpBuf, &stRawMsg, 10))
    {
        if (stRawMsg.eMsg == eUdpMsgGHOST)
        {
            uint32_t x, y;
            if (2 == scanMsg(stRawMsg.pMsgRaw, eUdpMsgGHOST, &y, &x)) //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
            {
                if (fantomIndex >= pCtx->stGameSet.fantomsMax)
                {
                    fantomIndex = 0;
                }

                pCtx->stGameSet.pFantoms[fantomIndex].uX = x;
                pCtx->stGameSet.pFantoms[fantomIndex].uY = y;
                MAZE_AT(pCtx->stGameSet.pMaze, pCtx->stGameSet.mazeW, x ,y) = eMazeEmpty;
                
                fantomIndex++;
            }
        }
        else if (stRawMsg.eMsg == eUdpMsgMESSA)
        {
            struct stMsg *pMsg = (struct stMsg*)malloc(sizeof(struct stMsg));
            if (pMsg)
            {
                if (2 == scanMsg(stRawMsg.pMsgRaw, eUdpMsgMESSA, pMsg->pId, pMsg->pMsg))
                {
                    pushLast(&pCtx->stGameSet.stListMsg, pMsg);
                }
                else
                {
                    free(pMsg);
                }
            }
        }
        else if (stRawMsg.eMsg == eUdpMsgENDGA)
        {
            char pMessage[512];
            char pId[USER_ID_LEN+1];
            uint32_t score = 0;

            if (2 != scanMsg(stRawMsg.pMsgRaw, eUdpMsgENDGA, pId, &score))
            {
                log_error("protocol error!\n");
            }

            printf("+++++++++++++++++++++++++++++++++++++++++++++++\n");
            printf("We have a winner %s!! the score is %u\n", pId, score);
            printf("+++++++++++++++++++++++++++++++++++++++++++++++\n");
            pCtx->bExit = true;
        }
        else if (stRawMsg.eMsg == eUdpMsgSCORE)
        {
            char pId[USER_ID_LEN+1];
            uint32_t score = 0, x = 0, y = 0;
            if (4 == scanMsg(stRawMsg.pMsgRaw, eUdpMsgSCORE, pId, &score, &y, &x))
            {
                printf("\nPlayer %s catch ghost at %ux%u, new score is %u!!\n", pId, x, y, score);
                for (uint32_t i = 0; i < pCtx->stGameSet.countPlayers; i++)
                {
                    if (0 == strcmp(pCtx->stGameSet.pPlayers[i].plId, pId))
                    {
                        pCtx->stGameSet.pPlayers[i].score = score;
                        break;    
                    }
                }
            }
        }
    }

    if (fantomIndex)
    {
        pCtx->stGameSet.fantoms = fantomIndex;
    }
}




//Extended ASCII codes
//http://gimnnik.narod.ru/open-office/TextProcessor/images/ascii2.gif
//https://www.utf8-chartable.de/unicode-utf8-table.pl?start=9600&number=128&names=-&utf8=string-literal
#define CHAR_GHOST   "@"
#define CHAR_PLAYER  "X"
#define CHAR_NEW_POS "+"
#define CHAR_COMP    "O"
#define CHAR_UNK     "\xe2\x96\x93"
#define CHAR_WALL    "\xe2\x96\x91"

void printMaze(struct stContext *pCtx)
{
    printf("=============LEGEND===========\n"
           " " CHAR_GHOST   " - ghost\n"
           " " CHAR_PLAYER  " - player \n"
           " " CHAR_NEW_POS " - move cursor \n"
           " " CHAR_COMP    " - competitors \n"
           " " CHAR_UNK     " - unknown \n"
           " " CHAR_WALL    " - wall \n");

    uint32_t uW = getTerminalWidth()-1;
    uint32_t uH = pCtx->stGameSet.mazeH;

    if (uW > pCtx->stGameSet.mazeW)
    {
        uW = pCtx->stGameSet.mazeW;
    }

    for (uint32_t uY = 0; uY < uH; uY++)
    {
        for (uint32_t uX = 0; uX < uW; uX++)
        {
            bool next = true;

            //print my icon
            if ((uX == pCtx->stGameSet.myX) && (uY == pCtx->stGameSet.myY))
            {
                setTerminalTextColor(eTcLightBlue);
                printf(CHAR_PLAYER);
                next = false;
            }

            if (!next) continue;

            //print ghost
            for (uint32_t j = 0; j < pCtx->stGameSet.fantoms; j++)
            {
                if ((uX == pCtx->stGameSet.pFantoms[j].uX) && (uY == pCtx->stGameSet.pFantoms[j].uY))
                {
                    setTerminalTextColor(eTcLightMagenta);
                    printf(CHAR_GHOST);
                    next = false;
                    break;
                }
            }

            if (!next) continue;

            //print players
            for (uint32_t i = 0; i < pCtx->stGameSet.countPlayers; i++)
            {
                if (    (pCtx->stGameSet.pPlayers[i].plId[0])
                     && (0 != strcmp(pCtx->stGameSet.pPlayers[i].plId, pCtx->userId))
                     && ((uX == pCtx->stGameSet.pPlayers[i].plX) && (uY == pCtx->stGameSet.pPlayers[i].plY))
                   )
                {
                    setTerminalTextColor(eTcYellow);
                    printf(CHAR_COMP);
                    next = false;
                    break;
                }
            }

            if (!next) continue;

            setTerminalTextColor(eTcLightGreen);

            enum eMaze eCell = MAZE_AT(pCtx->stGameSet.pMaze, 
                                       pCtx->stGameSet.mazeW, 
                                       uX,
                                       uY);

            if (eCell == eMazeEmpty)
            {
                printf(" ");
            }
            else if (eCell == eMazeWall)
            {
                printf(CHAR_WALL);
            }
            else
            {
                printf(CHAR_UNK);
            }
        }

        printf("\n"); 
        fflush(stdout);
    }

    fflush(stdout);

    setTerminalTextColor(DEFAULT_TERM_COLOR);
}

void sendUserMessage(struct stContext *pCtx)
{
    printf("Available addressee: \n");
    for (uint32_t i = 0; i < pCtx->stGameSet.countPlayers; i++)
    {
        printf(" #%d [%s]\n", i, pCtx->stGameSet.pPlayers[i].plId);
    }
    printf(" #%d All\n", (uint32_t)pCtx->stGameSet.countPlayers);

    printf("Please type addressee index: ");
    fflush(stdout);
    uint32_t index = read_input_uint();

    if (index > pCtx->stGameSet.countPlayers)
    {
       printf("ERROR, index %u is out of range [0 .. %u]\n", index, (uint32_t)pCtx->stGameSet.countPlayers);
       return;
    }

    printf("Please enter message (200 chars max): ");
    fflush(stdout);

    char message[MSG_MAX_TXT_LEN+64];
    read_input(message, sizeof(message));

    if (strstr(message, MSG_TCP_END) || strstr(message, MSG_UDP_END))
    {
        printf("ERROR, message contains unsupported characters *** or +++\n");
        return;
    }

    if (strlen(message) > MSG_MAX_TXT_LEN)
    {
        message[MSG_MAX_TXT_LEN] = 0;
    }

    struct stRawMessage stRawMsg;
    if (index == pCtx->stGameSet.countPlayers)
    {
        sendMsg(pCtx->tcpSocket, eTcpMsgMALLQ, message);
        if (socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stRawMsg))
        {
            if (stRawMsg.eMsg != eTcpMsgMALLA)
            {
                log_error("Receive unexpected message %d\n", stRawMsg.eMsg);
            }
        }   
    }
    else
    {
        sendMsg(pCtx->tcpSocket, eTcpMsgSENDQ, pCtx->stGameSet.pPlayers[index].plId, message);

        if (socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stRawMsg))
        {
            if (stRawMsg.eMsg == eTcpMsgSENDA)
            {
            }
            else if (stRawMsg.eMsg == eTcpMsgNSEND)
            {
                log_error("Can't send message to player \"%s\"", pCtx->stGameSet.pPlayers[index].plId);
            }
        }   
    }
}

// main function
void play(struct stContext *pCtx)
{
    bool redraw = true;
    while (!pCtx->bExit)
    {
        usleep(1000 * 10); //sleep 10ms

        receivePersonalMessages(pCtx);
        receiveMultiCastMessages(pCtx);

        if (pCtx->stGameSet.bCheat)
        {
            cheatGameSession(pCtx);

            pCtx->stGameSet.myNextMoveX = pCtx->stGameSet.myX;
            pCtx->stGameSet.myNextMoveY = pCtx->stGameSet.myY;

            usleep(1000 * 20);
        }


        if (redraw)
        {
            printf("\n=MENU=====================\n");
            printf("1 -> Move left\n");
            printf("2 -> Move right\n");
            printf("3 -> Move up\n");
            printf("4 -> Move down\n");
            printf("5 -> Print players\n");
            printf("6 -> Print maze\n");
            printf("7 -> On/off cheat mode\n");
            printf("8 -> Print messages\n");
            printf("9 -> print info\n");
            printf("m -> send message\n");
            printf("0 -> Exit game\n");
            printf("==========================\n");
            printf("Enter your choise: ");
            fflush(stdout);
            redraw = false;
        }

        if (!isKeyPressed()) continue;

        redraw = true;

        char ch = getchar();

        printf("\n");

        if ('0' == ch)
        {
            sendMsg(pCtx->tcpSocket, eTcpMsgIQUIT);
            struct stRawMessage stRawMsg;
            if (socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stRawMsg))
            {
                if (stRawMsg.eMsg == eTcpMsgGOBYE)
                {
                    printf("Goodby\n");
                    pCtx->bExit = true;
                }
            }
        }
        else if (('1' == ch) || ('2' == ch) || ('3' == ch) || ('4' == ch))
        {
            enum eMsgId msg = eUdpMsgMaxValue;
            if ('1' == ch)  msg = eTcpMsgLEMOV;
            else if ('2' == ch)  msg = eTcpMsgRIMOV;
            else if ('3' == ch)  msg = eTcpMsgUPMOV;
            else if ('4' == ch)  msg = eTcpMsgDOMOV;

            if (msg != eUdpMsgMaxValue)
            {
                printf("Please enter move offset: ");
                fflush(stdout);
                uint32_t move = read_input_uint();

                sendMsg(pCtx->tcpSocket, msg, move);

                struct stRawMessage stRawMsg;
                if (socketReadMsg(pCtx->tcpSocket, &pCtx->stTcpBuf, &stRawMsg))
                {
                    uint32_t uX, uY;
                    if (stRawMsg.eMsg == eTcpMsgMOVEA)
                    {
                        scanMsg(stRawMsg.pMsgRaw, eTcpMsgMOVEA, &uY, &uX); //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
                    }
                    else if (stRawMsg.eMsg == eTcpMsgMOVEF)
                    {
                        scanMsg(stRawMsg.pMsgRaw, eTcpMsgMOVEF, &uY, &uX, &pCtx->stGameSet.score); //TO RESPECT PROTOCOL AND RESPECT **Descartes** LEGACY!
                    }
                    else
                    {
                        log_error("Unexpected message %d", stRawMsg.eMsg);
                    }

                    //mark empty space of maze
                    while ((pCtx->stGameSet.myX != uX) || (pCtx->stGameSet.myY != uY))
                    {
                        if      (eTcpMsgDOMOV == msg) pCtx->stGameSet.myY++;
                        else if (eTcpMsgUPMOV == msg) pCtx->stGameSet.myY--;
                        else if (eTcpMsgLEMOV == msg) pCtx->stGameSet.myX--;
                        else if (eTcpMsgRIMOV == msg) pCtx->stGameSet.myX++;

                        MAZE_AT(pCtx->stGameSet.pMaze, pCtx->stGameSet.mazeW, pCtx->stGameSet.myX, pCtx->stGameSet.myY) = eMazeEmpty;
                        move--;
                    }

                    pCtx->stGameSet.myNextMoveX = pCtx->stGameSet.myX;
                    pCtx->stGameSet.myNextMoveY = pCtx->stGameSet.myY;

                    //if not all moves are made - mark a wall
                    if (move)
                    {
                        if      (eTcpMsgDOMOV == msg) uY++;
                        else if (eTcpMsgUPMOV == msg) uY--;
                        else if (eTcpMsgLEMOV == msg) uX--;
                        else if (eTcpMsgRIMOV == msg) uX++;
                        MAZE_AT(pCtx->stGameSet.pMaze, pCtx->stGameSet.mazeW, uX, uY) = eMazeWall;
                    }
                }
            }
        }
        else if ('5' == ch)
        {
            updatePlayersEx(pCtx);
            printPlayersEx(pCtx);
        }
        else if ('6' == ch)
        {
            printMaze(pCtx);
        }
        else if ('7' == ch)
        {
            pCtx->stGameSet.bCheat = ! pCtx->stGameSet.bCheat;
            if (pCtx->stGameSet.bCheat)
            {
                pCtx->stGameSet.pMazeCheat  = (enum eMaze*)malloc(pCtx->stGameSet.mazeW * pCtx->stGameSet.mazeH * sizeof(enum eMaze));
                if (!pCtx->stGameSet.pMazeCheat)
                {
                    log_error("Can't allocate memory");
                    pCtx->stGameSet.bCheat = false;
                }
                else
                {
                    for (uint32_t i = 0; i < pCtx->stGameSet.mazeW * pCtx->stGameSet.mazeH; i++)
                    {
                        pCtx->stGameSet.pMazeCheat[i] = eMazeUnknown;
                    }

                    MAZE_AT(pCtx->stGameSet.pMazeCheat, pCtx->stGameSet.mazeW, pCtx->stGameSet.myX, pCtx->stGameSet.myY) = eMazeEmpty;
                }
            }
            else
            {
                FREE(pCtx->stGameSet.pMazeCheat);
                cheatPathClear(pCtx);
            }
        }
        else if ('8' == ch)
        {
            printf("Messages: \n");
            while (pCtx->stGameSet.stListMsg.first)
            {
                struct stMsg *pMsg = (struct stMsg *)pCtx->stGameSet.stListMsg.first->data;
                printf("Message from %s\n------------------------------\n%s\n------------------------------\n",
                       pMsg->pId,
                       pMsg->pMsg
                      );
                free(pMsg);
                removeEl(&pCtx->stGameSet.stListMsg, pCtx->stGameSet.stListMsg.first);
            }
        }
        else if ('9' == ch)
        {
            printf("==============INFO============\n"
                   " * Fantoms %u\n"
                   " * Score %u\n"
                   " * Coordinates %ux%u\n",
                   pCtx->stGameSet.fantoms,
                   pCtx->stGameSet.score,
                   (uint32_t)pCtx->stGameSet.myX,
                   (uint32_t)pCtx->stGameSet.myY
                  );
        }
        else if (('m' == ch) || ('M' == ch))
        {
            sendUserMessage(pCtx);
        }
    }
}