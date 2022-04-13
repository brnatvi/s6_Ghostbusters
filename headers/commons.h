#ifndef COMMONS
#define COMMONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "list.h"

#include <time.h>

#define SIZE_ID     8
#define BUF_SIZE    512


////////////////////////////////// Structures ////////////////////////////////////
struct stGamer
{                                       // for protocol
    char*       id;                        //char[8];          // id of gamer
    uint32_t    IP;                        //char[15]          // IP of gamer
    uint16_t    x;                         //char[3];          // position x
    uint16_t    y;                         //char[3];          // position y
    uint16_t    score;                     //char[4];          // score  
    uint32_t    fd2;    
};

struct stGhost                 
{                   
    uint16_t    id;                                            // id of ghost
    uint16_t    x;                         //char[3];          // position x
    uint16_t    y;                         //char[3];          // position y
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
    pthread_mutex_t     lock;  
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
};

struct stConnection
{       
    int                    fd1;    
    struct sockaddr_in     sockAddress;  
    struct listElements_t* games;  
    uint16_t               lastGameId;
};

struct stCommunication
{       
    struct stGamer*        gamer;
    struct stConnection*   connection;   
};

////////////////////////////////// Functions ////////////////////////////////////

int   createTcpConnection(struct stConnection * connect, int port);
int   acceptAndCommunication(struct stConnection* connect);
void* communication(void *args);
struct stGame* createGame(struct stCommunication *context, char* bufer);


////////////////////////////////// Macros' //////////////////////////////////////

#define CLOSE(File) if (File >= 0) {close(File); File = -1; }
#define FREE_MEM(Mem) if (Mem) {free(Mem); Mem = NULL;}

#endif //COMMONS