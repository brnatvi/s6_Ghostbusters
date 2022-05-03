#include "server.h"
#include "commons.h"

//////////////////////////// Main functions /////////////////////////////////

int   createTcpConnection(struct stServerContext *context, int port);
int   acceptAndCommunication(struct stServerContext *context);
void *routine_com(void *args);


//////////////////////////// Process functions //////////////////////////////

bool sendGames(struct stGamerContext *gContext);
bool processNEWPL(struct stGamerContext *gContext, uint8_t *bufer);
bool processREGIS(struct stGamerContext *gContext, uint8_t *bufer);
bool processUNREG(struct stGamerContext *gContext, uint8_t *bufer);
bool processSIZE_(struct stGamerContext *gContext, uint8_t *bufer);
bool processLIST_(struct stGamerContext *gContext, uint8_t *bufer);
//bool processGAMES(struct stGamerContext *gContext, uint8_t *bufer);

