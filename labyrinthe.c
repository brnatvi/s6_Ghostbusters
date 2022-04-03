#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

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
    eCellEmpty,
    eCellGamer,
    eCellGhost,
    eCellWall,
};

struct stCell
{
    enum eCellType eType;
    struct stGamer *gamer;
    struct stGhost *ghost;
    pthread_mutex_t lock;  
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

            if((j==0 && i!=3) || (j==1 && i==5) ||
            (j==2 && i != 4) || 
            (j==3 && i==0) || (j==3 && i==2) || (j==3 && i==5) ||
            (j == 4 && i == 0) || (j == 4 && i == 5) ||
            (j==5 && i==0) || (j==5 && i==2) || (j==5 && i==3) ||
            (j==6 && i!=1)){
                lab[i][j].eType = eCellWall;
                //lab[i][j].gamer = ;
                //lab[i][j].ghost = ;
            }
            else {
                lab[i][j].eType = eCellEmpty;  
            }
            
        }
    }
    return lab;
}

void print_maze(int lines, int columns, char *id){
    stCell** lab = generate_maze(lines,columns);
    for(int i = -1; i<lines; i++){
        for(int j = -1; j<columns; j++){
            if(i == -1 && j == -1) {
                printf("\t");
            }
            else if(i == -1){
                printf("%d\t",j);
            }
            else if(j == -1){
                printf("%d\t", i);
            }
            
            else {
                if(strcmp(lab[i][j].gamer->id, id)){
                    printf("%d\t", lab[i][i].eType); //le joueur ne voit que sa position
                }
                else {
                    printf("%d\t", 0); //le client ne voit pas les autres joueurs, ni les fantômes, ni les murs
                }
                
            }
            
            
            
        }
        printf("\n");
    }
}

int main(void){
    print_maze(6,7, "tharsiya");
}

