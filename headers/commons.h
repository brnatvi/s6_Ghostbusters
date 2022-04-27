#ifndef COMMONS
#define COMMONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "list.h"

#include <time.h>

#define LEN_ID          8           // length of gamer's id (preudo)
#define LEN_KEYWORD     5           // length of keywords
#define LEN_PORT        4           // length of port in string
#define BUF_SIZE        6098


////////////////////////////////// Structures ////////////////////////////////////
struct stGamer
{                                                // for protocol
    char                id[LEN_ID+1];              //char[8];          // id of gamer
    uint32_t            ipAddress;                 //char[15]          // IP adresse of gamer
    uint16_t            portUDP;                   //char[4]           // UDP port of gamer
    uint16_t            x;                         //char[3];          // position x
    uint16_t            y;                         //char[3];          // position y
    uint16_t            score;                     //char[4];          // score  
    int32_t             fd2;    
    pthread_mutex_t     gamerLock;
};

struct stGhost                 
{                   
    uint16_t            id;                                            // id of ghost
    uint16_t            x;                         //char[3];          // position x
    uint16_t            y;                         //char[3];          // position y
};

enum eCellType
{
    eCellGamer,
    eCellGhost,
    eCellWall,
    eCellEmpty
};
struct stCell
{
    enum eCellType      eType;
    struct stGamer*     gamer;
    struct stGhost*     ghost;    
};

struct stLabirinth                  
{                   
    uint16_t               heigh;              //uint_16;         // heigh
    uint16_t               width;              //uint_16;         // width  
    uint8_t                ghostCount;                            //number of ghosts
    struct listElements_t* ghosts;
    struct stCell*         grid;                                  //labirynth  
};

struct stGame
{       
    uint8_t                idGame;
    struct listElements_t* gamers;    
    struct stLabirinth*    labirinth;
    uint16_t               port;              //char[4]
    uint8_t                isLaunched;  
    pthread_mutex_t        gameLock;
};

struct stServerContext                        //Created by: server main 
{                                             //Instances : single per process
    int                    fd1;               //Access    : server main (thread main)
    struct sockaddr_in     sockAddress;       //          : communication (thread comm)
    struct listElements_t* games;             //          : all protocol functions (thread comm)
    uint16_t               lastGameId;        //          : changed by createGame (thread comm)    
    uint8_t                countStarted;
    pthread_mutex_t        serverLock;
    struct listElements_t* users;
};

struct stGamerContext
{                                      
    struct stServerContext *pServerCtx;
    struct stGamer         *pGamer;
};

////////////////////////////////// Functions ////////////////////////////////////
int createTcpConnection(struct stServerContext *context, int port);
int acceptAndCommunication(struct stServerContext *context);


//////////////////////////// Aux functions /////////////////////////////////

size_t recieveMessage(int fd, char *bufer, char* ending);
struct stGame *createGame(struct stGamerContext *gContext, char *bufer);
void freeGame(struct stGame *pGame);

struct stGamer *createGamer(int32_t iSocket, uint32_t uIpv4);
void freeGamer(struct stGamer *pGamer);

void addGamer(struct stGamerContext *gContext, struct stGame *game, struct stGamer *gamer);
void removeGamer(struct stGamerContext *gContext, struct stGame *game, struct stGamer *gamer);
void addGame(struct stGamerContext *gContext, struct stGame *newGame);
void removeGame(struct stGamerContext *gContext, struct stGame *game);

void printGamers(struct stGamerContext *gContext, const char* pCaller);


////////////////////////////////// Macros' //////////////////////////////////////

#define CLOSE(File) if (File >= 0) {close(File); File = -1; }
#define FREE_MEM(Mem) if (Mem) {free(Mem); Mem = NULL;}

#endif //COMMONS