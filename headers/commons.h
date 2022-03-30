#ifndef COMMONS
#define COMMONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <time.h>

struct stGamer
{                                       // for protocol
    uint16_t id;                        //char[8];          // id of gamer
    uint32_t IP;                        //char[15]          // IP of gamer
    uint16_t x;                         //char[3];          // position x
    uint16_t y;                         //char[3];          // position y
    uint16_t score;                     //char[4];          // score  
};                  

struct stGhost                  
{                   
    uint16_t id;                        //char[8];          // id of ghost
    uint16_t x;                         //char[3];          // position x
    uint16_t y;                         //char[3];          // position y
};                  

struct stLabirinth                  
{                   
    uint16_t heigh;                      //uint_16;         // heigh
    uint16_t width;                      //uint_16;         // width    
    struct listElements_t *ghosts;      
};

struct stGame
{       
    struct listElements_t *gamers;    
    struct stLabirinth    *labirinth;
    uint16_t               port;        //char[4]
};

#define CLOSE(File) if (File >= 0) {close(File); File = -1; }
#define FREE_MEM(Mem) if (Mem) {free(Mem); Mem = NULL;}

#endif //COMMONS