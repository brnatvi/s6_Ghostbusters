#include "server.h"
#include "commons.h"

//////////////////////////// Main functions /////////////////////////////////

int createTcpConnection(struct stServerContext *context, int port);
int acceptAndCommunication(struct stServerContext *context);
void *communication(void *args);


//////////////////////////// Process functions //////////////////////////////

ssize_t processINFO(struct stGamerContext *gContext, ssize_t rezSend, char *answer);
ssize_t processNEWPL(struct stGamerContext *gContext, char *bufer, ssize_t rezSend, char* answer);

ssize_t processREGIS(struct stGamerContext *gContext, char *bufer, ssize_t rezSend, char *answer);
ssize_t processUNREG(struct stGamerContext *gContext, char *bufer, ssize_t rezSend, char *answer);
ssize_t processSIZE_(struct stGamerContext *gContext, char *bufer, ssize_t rezSend, char *answer);
ssize_t processLIST_(struct stGamerContext *gContext, char *bufer, ssize_t rezSend, char *answer);
ssize_t processGAME_(struct stGamerContext *gContext, ssize_t rezSend, char *answer);
ssize_t processGAMES(struct stGamerContext *gContext, char *bufer, ssize_t rezSend, char *answer);

//////////////////////////// Aux functions /////////////////////////////////

size_t recieveMessage(int fd, char *bufer, char* ending);
struct stGame *createGame(struct stGamerContext *gContext, char *bufer);

struct stGamer *createGamer(int32_t iSocket, uint32_t uIpv4);
void freeGamer(struct stGamer *pGamer);

void addGamer(struct stGamerContext *gContext, struct stGame *game, struct stGamer *gamer);
void removeGamer(struct stGamerContext *gContext, struct stGame *game, struct stGamer *gamer);
void addGame(struct stGamerContext *gContext, struct stGame *newGame);
void removeGame(struct stGamerContext *gContext, struct stGame *game);

void printGamers(struct stGamerContext *gContext, const char* pCaller);