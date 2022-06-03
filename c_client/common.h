#ifndef CLI_COMMON
#define CLI_COMMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <time.h>

#include "list.h"
#include "socket.h"
#include "protocol.h"
#include "log.h"

//activate to show protocol messages
#define VERBOSE_TCP_PROTOCOL
//#define VERBOSE_UDP_PROTOCOL

#define DEFAULT_TERM_COLOR eTcWhite

struct stPlId
{
    char pId[USER_ID_LEN+1];
};

struct stGameInfo
{
    uint8_t        cId;
    uint8_t        countPlayers;
    struct stPlId *pPlayers;
    uint16_t       width;
    uint16_t       heigth;
};

struct stPlayer
{
    char     plId[USER_ID_LEN+1];
    uint32_t score;
    uint32_t plX;
    uint32_t plY;
};

struct stFantom
{
    uint32_t uX;
    uint32_t uY;
};

struct stMsg
{
    char pId[USER_ID_LEN+1];
    char pMsg[MSG_MAX_LEN];
};

enum eMaze
{
    eMazeEmpty = 0,
    eMazeWall,
    eMazeUnknown,
};

struct stCheatCoord
{
    uint32_t uX;
    uint32_t uY;
};


struct stGame
{
    struct listElements_t stListMsg;     //list of stMsg
    int                   udpMctSocket;
    struct stIpBuffer     stUdpBuf;

    uint8_t               cId;

    uint8_t               countPlayers;
    struct stPlayer      *pPlayers;

    uint32_t              fantoms;
    uint32_t              fantomsMax;
    struct stFantom      *pFantoms;

    uint32_t              mazeW;
    uint32_t              mazeH;
    enum eMaze           *pMaze;

    bool                  bCheat;
    enum eMaze           *pMazeCheat;
    struct listElements_t listCheatPath; //stCoord*
    uint32_t              cheatSpeed;

    uint32_t              myX;
    uint32_t              myY;
    uint32_t              myNextMoveX;
    uint32_t              myNextMoveY;

    uint32_t              score;

    uint32_t              gameCycle;
};


struct stContext
{
    int                   tcpSocket;
    int                   udpPeerSocket;
    uint16_t              udpPeerPort;
    struct stIpBuffer     stTcpBuf;
    struct stIpBuffer     stUdpBuf;
                         
    uint32_t              cliW;
    uint32_t              cliH;
    char                  userId[USER_ID_LEN + 1];
                 
    uint8_t               gamesCount;
    struct stGameInfo    *pGames;
    struct stGameInfo    *pSelectedGame;

    struct stGame         stGameSet;
    bool                  bExit;
};

#define MAZE_AT(STRUCT, W, X, Y) STRUCT[(W) * (Y) + (X)]

void cheatPathAdd(struct stContext *pCtx, uint32_t X, uint32_t Y);
const struct stCheatCoord* cheatPathGetLast(struct stContext *pCtx);
void cheatPathDelLast(struct stContext *pCtx);
void cheatPathClear(struct stContext *pCtx);
void cheatGameSession(struct stContext *pCtx);

#include "utils.h"
#include "engine.h"

#define SHUTDOWN_EXIT(File) if (File >= 0) {shutdown(File, SHUT_RDWR); close(File); File = -1;}
#define CLOSE(File) if (File >= 0) {close(File); File = -1; }
#define FREE(Mem) if (Mem) free(Mem); Mem = NULL;

#define NULL_DESCRIPTOR -1

#endif //CLI_COMMON