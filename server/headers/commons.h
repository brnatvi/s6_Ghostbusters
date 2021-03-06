#ifndef COMMONS
#define COMMONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#include "list.h"
#include "log.h"
#include "maze_generator.h"
#include "protocol.h"

#define LEN_PORT        4           // length of port in string
#define BUF_SIZE        6098

#define MULTI_CAST_ADDR "239.255.255.250"
#define GHOST_MOVEMENT_INTERVAL_MS 10000
#define MIN_MAZE_SIZE 10

//activate to show protocol messages
#define VERBOSE_TCP_PROTOCOL
#define VERBOSE_UDP_PROTOCOL

////////////////////////////////// Structures ////////////////////////////////////

struct stGame;

struct stPlayer
{                                               
    int32_t             tcpSocket;
    uint32_t            ipAddress;
    uint16_t            portUDP;
    struct stIpBuffer   stTcpBuf;

    char                id[USER_ID_LEN+1];

    uint16_t            x;
    uint16_t            y;
    uint16_t            score;

    struct stGame      *pGame;
    bool                isStarted;
};

struct stGhost
{                   
    uint16_t            x;                         //char[3];          // position x
    uint16_t            y;                         //char[3];          // position y
    uint8_t            id;                                             // id of ghost
};

#define MAZE_WALL 1

struct stLabirinth                  
{                   
    uint16_t               heigh;              //uint_16;         // heigh
    uint16_t               width;              //uint_16;         // width  
    struct listElements_t *ghosts;
    unsigned int          *maze;              //labirynth  [w][w][w][w] x Heigh
};


struct stGame
{       
    uint8_t                idGame;
    struct listElements_t* lstPlayers;
    struct stLabirinth     labirinth;
    int                    udpMsgSocket;
    int                    udpMctSocket;
    struct sockaddr_in     MctAddr;
    uint8_t                isLaunched;   
    pthread_mutex_t        gameLock;
    pthread_t              thread;
};

struct stServerContext                        //Created by: server main 
{                                             //Instances : single per process
    int                    tcpListenSocket;   //Access    : server main (thread main)
                                              //          : routine_com (thread comm)
    struct sockaddr_in     sockAddress;       //          : all protocol functions (thread comm)
    struct listElements_t* lstGames;          //          : changed by createGame (thread comm)    
    uint16_t               lastGameId;        
    uint8_t                countStarted;
    pthread_mutex_t        serverLock;
    uint16_t               mazeSzLimite;
};

struct stGamerContext
{                                      
    struct stServerContext *serverCtx;
    struct stPlayer        *player;
};

////////////////////////////////// Functions ////////////////////////////////////
int createTcpConnection(struct stServerContext *context, int port);
int acceptAndCommunication(struct stServerContext *context);


//////////////////////////// Aux functions /////////////////////////////////

size_t recieveMessage(int fd, char *bufer, char* ending);
struct stGame *createGame(struct stGamerContext *gContext);
void freeGame(struct stGame *pGame);

struct stPlayer *createPlayer(int32_t iSocket, uint32_t uIpv4);
void freePlayer(struct stPlayer *player);

bool addGamePlayer(struct stGamerContext *gContext, struct stGame *game, struct stPlayer *gamer);
bool removeGamePlayer(struct stGamerContext *gContext, struct stPlayer *player);

void addGame(struct stGamerContext *gContext, struct stGame *newGame);
bool removeGame(struct stGamerContext *gContext, struct stGame *game, bool bForce);

bool isGameReadyToStart(struct stGamerContext *gContext, struct stGame *game);

void printPlayers(struct stGamerContext *gContext, const char* pCaller);


////////////////////////////////// Macros' //////////////////////////////////////
#define MAZE_AT(STRUCT, W, X, Y) STRUCT[(W) * (Y) + (X)]

#define CLOSE(File) if (File >= 0) {close(File); File = -1; }
#define FREE_MEM(Mem) if (Mem) {free(Mem); Mem = NULL;}


enum eTermColor
{
    //https://en.wikipedia.org/wiki/ANSI_escape_code
    eTcBlack        = 0,
    eTcBlue         = 12,
    eTcGreen        = 40,
    eTcCyan         = 51,
    eTcRed          = 124,
    eTcMagenta      = 129,
    eTcBrown        = 94,
    eTcLightGray    = 252,
    eTcDarkGray     = 244,
    eTcLightBlue    = 33,
    eTcLightGreen   = 48,
    eTcLightCyan    = 195,
    eTcLightRed     = 196,
    eTcLightMagenta = 204,
    eTcYellow       = 11,
    eTcWhite        = 15
};

#define SET_TERMINAL_COLOR(X) printf("\x1b[38;5;%dm", X);
#define DEFAULT_TERM_COLOR eTcWhite

#endif //COMMONS