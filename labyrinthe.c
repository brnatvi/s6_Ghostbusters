#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

typedef struct stGamer stGamer;
typedef struct stGhost stGhost;
typedef struct stCell stCell;
typedef struct stMaze stMaze;

struct stGamer
{                                       // for protocol
    char     id[8];                     //char[8];          // id of gamer
    uint32_t IP;                        //char[15]          // IP of gamer
    uint16_t x;                         //char[3];          // position x
    uint16_t y;                         //char[3];          // position y
    uint16_t score;                     //char[4];          // score  
};                  

struct stGhost                 
{                   
    uint16_t id;                        //******            // id of ghost
    uint16_t x;                         //char[3];          // position x
    uint16_t y;                         //char[3];          // position y
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
    int nb;
    // enum eCellType eType;
    // struct stGamer *gamer;
    // struct stGhost *ghost;
    // pthread_mutex_t lock;  
};

struct stMaze                  
{                   
    uint16_t height;                      //uint_16;         // heigh
    uint16_t width;                      //uint_16;         // width  
    uint8_t ghostCount;                  //number of ghosts
    struct listElements_t *ghosts;
    stCell *grid;                 //labirynth
};

stCell **generate_maze(int lines, int columns){
    stCell **lab;
    lab = malloc(sizeof(stCell*)*lines);
    for(int i=0; i<lines; i++){
        lab[i] = malloc(sizeof(stCell*) *columns);
    }

    for(int i = 0; i<lines; i++){
        for(int j = 0; j<columns; j++){
            lab[i][j].nb = 0;  
        }
    }
    return lab;
}

void print_maze(int lines, int columns){
    stCell** lab = generate_maze(lines,columns);
    for(int i = -1; i<lines; i++){
        for(int j = -1; j<columns; j++){
            if(i == -1 && j == -1) {
                printf("\t");
            }
            else if(i == -1){
                printf("%d\t",j+1);
            }
            else if(j == -1){
                printf("%d\t", i+1);
            }
            
            else {
                printf("%d\t", lab[i][j].nb);
            }
            
            
            
        }
        printf("\n");
    }
}

int main(void){
    print_maze(6,7);
}

