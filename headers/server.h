#ifndef SERVER
#define SERVER

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define TCP_END "***"
#define UDP_END "+++"

#define GAMES   "GAMES "
#define OGAME   "OGAME "
#define NEWPL   "NEWPL "
#define REGIS   "REGIS "
#define REGOK   "REGOK "
#define REGNO   "REGNO"
#define START   "START"
#define NMEMB   "NMEMB"
#define UNREG   "UNREG"
#define UNROK   "UNROK "
#define DUNNO   "DUNNO"
#define SIZE_Q  "SIZE? "
#define SIZE_E  "SIZE! "
#define LIST_Q  "LIST? "
#define LIST_E  "LIST! "
#define PLAYR   "PLAYR "
#define GAME_Q  "GAME?"
#define WELCO   "WELCO"
#define POSIT   "POSIT "
#define UPMOV   "UPMOV "
#define DOMOV   "DOMOV "
#define LEMOV   "LEMOV "
#define RIMOV   "RIMOV "
#define MOVE_E  "MOVE! "
#define MOVEF   "MOVEF "
#define IQUIT   "IQUIT"
#define GOBYE   "GOBYE"
#define GLIS_Q  "GLIS?"
#define GLIS_E  "GLIS! "
#define GPLYR   "GPLYR "
#define MALL_Q  "MALL? "
#define MALL_E  "MALL!"
#define SEND_Q  "SEND? "
#define SEND_E  "SEND!"
#define NSEND   "NSEND"

#define GHOST   "GHOST "
#define SCORE   "SCORE "
#define MESSA   "MESSA "
#define ENDGA   "ENDGA "
  
#define MESSP   "MESSP "


#endif //SERVER