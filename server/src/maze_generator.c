#include "maze_generator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum eCellType {eCELL_BASE = 0, eCELL_VISITED, eCELL_WALL};

typedef int** tMazeMatrix;

struct stMaze_Cell
{
    unsigned int x;
    unsigned int y;
};

struct stMazeCellsArray
{
    struct stMaze_Cell *pCell;
    unsigned int        size;
};

struct stCellStack
{
    struct stMaze_Cell  stCell;
    struct stCellStack *pNext;
};


struct stMazeContext
{
    unsigned int        width;
    unsigned int        height;
    unsigned int        unvisitedNum;
    tMazeMatrix         pMatrix;
    struct stCellStack *pStack;
    unsigned int        stackSize;
    struct stMaze_Cell  startPoint;
    struct stMaze_Cell  cellNext;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stack
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void stack_push(struct stMaze_Cell c, struct stCellStack **s, unsigned int* pSize)
{ 
   struct stCellStack *tmp = (struct stCellStack *)malloc(sizeof(struct stCellStack));
   tmp->pNext = *s;
   tmp->stCell = c;
   (*pSize)++;
   *s = tmp;
}


struct stMaze_Cell stack_pop(struct stCellStack **s, unsigned int* pSize)
{
    struct stCellStack* out;
    struct stMaze_Cell c;
    out = *s;
    *s = (*s)->pNext;
    c = out->stCell;
    (*pSize)--;
    free(out);
    return c;
}


void stack_clear(struct stCellStack **s, unsigned int *pSize)
{
    while(*pSize > 0)
    {
        if((*s)->pNext != NULL)
        {
            stack_pop(s, pSize);
        }
        else
        {
            free(*s);
            *s = NULL;
            (*pSize) = 0;
            break;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Service functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tMazeMatrix maze_init_matrix(unsigned int width, unsigned int height)
{ 
    unsigned int i, j;
    tMazeMatrix maze = (tMazeMatrix)malloc(height * sizeof(int*));

    for(i = 0; i < height; i++)
    {   //initialize matrix
        maze[i] = (int*)malloc(width * sizeof(int));
        for(j = 0; j < width; j++)
        {
            maze[i][j] = eCELL_WALL;
        }
    }

    for(i = 1; i < height-1; i+=2)
    {   //initialize cells
        for(j = 1; j < width-1; j+=2)
        {
            maze[i][j] = eCELL_BASE;
        }
    }
    return maze;
}


void maze_setMode(struct stMaze_Cell c, tMazeMatrix maze, int mode)
{ 
    maze[c.y][c.x] = mode;
}


unsigned int maze_getUnvisitedCount(unsigned int width, unsigned int height, tMazeMatrix maze)
{ 
    unsigned int count = 0, i, j;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            if(    (maze[i][j] != eCELL_WALL)
                && (maze[i][j] != eCELL_VISITED)
              )
            {
                count++;
            }
        }
    }
    return count;
}


struct stMazeContext maze_initialize(int width, int height, struct stMaze_Cell startPoint)
{   //used
    struct stMazeContext d;
    d.width = width;
    d.height = height;
    d.startPoint.x = startPoint.x;
    d.startPoint.y = startPoint.y;
    d.pMatrix = maze_init_matrix(width, height);
    maze_setMode(d.startPoint, d.pMatrix, eCELL_VISITED);
    d.pStack = NULL;
    d.stackSize = 0;
    d.unvisitedNum = maze_getUnvisitedCount(width, height, d.pMatrix);
    return d;
}


void maze_destroy(struct stMazeContext *pMaze)
{
    if (!pMaze)
    {
        return;
    }

    if (pMaze->pMatrix)
    {
        for(unsigned int i = 0; i < pMaze->height; i++)
        { 
            free(pMaze->pMatrix[i]);
        }

        free(pMaze->pMatrix);
    }

    if (pMaze->pStack)
    {
        stack_clear(&pMaze->pStack, &pMaze->stackSize);
    }
}


#define NEIGHBOURS_COUNT 4
struct stMazeCellsArray maze_getNeighbours(unsigned int width, unsigned int height, tMazeMatrix maze, struct stMaze_Cell c, int distance)
{
    unsigned int i;
    struct stMaze_Cell up = {c.x, c.y - distance};
    struct stMaze_Cell rt = {c.x + distance, c.y};
    struct stMaze_Cell dw = {c.x, c.y + distance};
    struct stMaze_Cell lt = {c.x - distance, c.y};
    struct stMaze_Cell d[NEIGHBOURS_COUNT]  = {dw, rt, up, lt};
    unsigned int size = 0;

    struct stMazeCellsArray cells;
    cells.pCell = (struct stMaze_Cell*)malloc(NEIGHBOURS_COUNT * sizeof(struct stMaze_Cell));

    for(i = 0; i < NEIGHBOURS_COUNT; i++)
    { 
        if(d[i].x > 0 && d[i].x < width && d[i].y > 0 && d[i].y < height)
        { 
            unsigned int mazeCellCurrent = maze[d[i].y][d[i].x];
            struct stMaze_Cell     cellCurrent     = d[i];
            if(mazeCellCurrent != eCELL_WALL && mazeCellCurrent != eCELL_VISITED)
            {   //not visited and it is a wall
                cells.pCell[size] = cellCurrent; 
                size++;
            }
        }
    }

    cells.size = size;
    return cells;
}


unsigned int maze_randomRange(unsigned int low, unsigned int high)
{
    return rand() % (high - low + 1) + low;
}


tMazeMatrix maze_removeWall(struct stMaze_Cell first, struct stMaze_Cell second, tMazeMatrix maze)
{ 
    short int xDiff = second.x - first.x;
    short int yDiff = second.y - first.y;
    short int addX, addY;
    struct stMaze_Cell target;

    addX = (xDiff != 0) ? (xDiff / abs(xDiff)) : 0;
    addY = (yDiff != 0) ? (yDiff / abs(yDiff)) : 0;

    target.x = first.x + addX; //wall coordinate
    target.y = first.y + addY;

    maze[target.y][target.x] = eCELL_VISITED;
    return maze;
}


struct stMazeCellsArray maze_getUnvisitedCells(unsigned int width, unsigned int height, tMazeMatrix maze)
{
    struct stMazeCellsArray cells;
    unsigned int i, j, size = 0;

    cells.pCell = (struct stMaze_Cell*)malloc(width*height*sizeof(struct stMaze_Cell));

    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            if((maze[i][j] != eCELL_WALL) && (maze[i][j] != eCELL_VISITED))
            {
                cells.pCell[size].x = j;
                cells.pCell[size].y = i;
                size++;
            }
        }
    }
    cells.size = size;
    return cells;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void maze_generateStep(struct stMazeContext *pMaze)
{
    unsigned int randNum;
    struct stMazeCellsArray cellStringUnvisited;
    struct stMazeCellsArray cellStringNeighbours = maze_getNeighbours(pMaze->width, pMaze->height, pMaze->pMatrix, pMaze->startPoint, 2);
    if (cellStringNeighbours.size)
    {   //if cell has unvisited neighbors
        randNum  = maze_randomRange(0, cellStringNeighbours.size-1);
        pMaze->cellNext = cellStringNeighbours.pCell[randNum]; //select random neighbors
        stack_push(pMaze->startPoint, &pMaze->pStack, &pMaze->stackSize); //push current cell in stack
        pMaze->pMatrix = maze_removeWall(pMaze->startPoint, pMaze->cellNext, pMaze->pMatrix); //remove wall between current and neighboring cells
        pMaze->unvisitedNum--;
        pMaze->startPoint = pMaze->cellNext; //make neighboring cell the current one and mark as visited
        maze_setMode(pMaze->startPoint, pMaze->pMatrix, eCELL_VISITED);
    }
    else if (pMaze->stackSize)
    {   //if there is no neighbors - comeback to prev. one
        pMaze->startPoint = stack_pop(&pMaze->pStack, &pMaze->stackSize);
    }
    else
    {   //if there is no neighbors and cells in stack, but not all cells are visited - select one randomly
        cellStringUnvisited = maze_getUnvisitedCells(pMaze->width, pMaze->height, pMaze->pMatrix);
        if (cellStringUnvisited.size)
        {
            randNum             = maze_randomRange(0, cellStringUnvisited.size-1);
            pMaze->startPoint      = cellStringUnvisited.pCell[randNum];
        }

        if (cellStringUnvisited.pCell)
        {
            free(cellStringUnvisited.pCell);
            cellStringUnvisited.pCell = NULL;
        }
    }

    if (cellStringNeighbours.pCell)
    {
        free(cellStringNeighbours.pCell);
        cellStringNeighbours.pCell = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int *maze_create(size_t szW, size_t szH)
{
    struct stMaze_Cell startPoint;

    startPoint.x = 1;
    startPoint.y = 1;


    srand((unsigned int)time(NULL)); 

    struct stMazeContext d = maze_initialize((int)szW, (int)szH, startPoint);

    do
    {
        maze_generateStep(&d);
    }
    while(d.unvisitedNum > 0);

    unsigned int *pReturn = (unsigned int *)malloc(szW * szH * sizeof(unsigned int));

    for (unsigned int uH = 0; uH < d.height; uH++)
    {
        for (unsigned int uW = 0; uW < d.width; uW++)
        {
            if (eCELL_WALL == d.pMatrix[uH][uW])
            {
                pReturn[(size_t)uH * szW + (size_t)uW] = 1;
            }
            else
            {
                pReturn[(size_t)uH * szW + (size_t)uW] = 0;
            }
        }
    }

    maze_destroy(&d);

    return pReturn;
}
