#include "server.h"
#include "commons.h"

//////////////////////////// Main functions /////////////////////////////////

int createTcpConnection(struct stConnection *connect, int port);
int acceptAndCommunication(struct stConnection *connect);
void *communication(void *args);


//////////////////////////// Process functions //////////////////////////////

ssize_t processINFO(struct stCommunication *context, ssize_t rezSend, char* answer);
ssize_t processNEWPL(struct stCommunication *context, char *bufer, ssize_t rezSend, char* answer);

ssize_t processREGIS(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer);
ssize_t processUNREG(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer);
ssize_t processSIZE_(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer);
ssize_t processLIST_(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer);
ssize_t processGAME_(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer);
ssize_t processGAMES(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer);

//////////////////////////// Aux functions /////////////////////////////////

size_t recieveMessage(int fd, char *bufer, char* ending);
struct stGame *createGame(struct stCommunication *context, char *bufer);