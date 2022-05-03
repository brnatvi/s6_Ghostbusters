#ifndef COMMONS
#define COMMONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#include "list.h"
#include "maze_generator.h"
#include "protocol.h"

#define LEN_PORT        4           // length of port in string
#define BUF_SIZE        6098


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
    uint16_t               port;              //char[4]
    uint8_t                isLaunched;   //!!!USE SERVER LOCK TO UPDATE! 
    pthread_mutex_t        gameLock;
};

struct stServerContext                        //Created by: server main 
{                                             //Instances : single per process
    int                    tcpListenSocket;  //Access    : server main (thread main)
                                              //          : routine_com (thread comm)
    struct sockaddr_in     sockAddress;       //          : all protocol functions (thread comm)
    struct listElements_t* lstGames;             //          : changed by createGame (thread comm)    
    uint16_t               lastGameId;        
    uint8_t                countStarted;
    pthread_mutex_t        serverLock;
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

void printPlayers(struct stGamerContext *gContext, const char* pCaller);


////////////////////////////////// Macros' //////////////////////////////////////

#define CLOSE(File) if (File >= 0) {close(File); File = -1; }
#define FREE_MEM(Mem) if (Mem) {free(Mem); Mem = NULL;}

#endif //COMMONS