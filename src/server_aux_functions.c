#include "server_aux_functions.h"

// IMPORTANT!!! to test with nc dont use <enter> to send, but <ctrl d> (probleme of \0 ending)

// create socket, bind, listen
// return 0 (success) or -1 (failure)
int createTcpConnection(struct stConnection *connect, int port)
{
    int rez = EXIT_SUCCESS;

    // fill sockaddr_in
    connect->sockAddress.sin_family = AF_INET;
    connect->sockAddress.sin_port = htons(port);              // TODO check that need htons
    connect->sockAddress.sin_addr.s_addr = htonl(INADDR_ANY); // TODO check that need htons

    //create a socket
    connect->fd1 = socket(AF_INET, SOCK_STREAM, 0);
    if (connect->fd1 < 0)
    {
        perror("P: cannot create socket");
        rez = -1;
        goto lExit;
    }

    rez = bind(connect->fd1, (struct sockaddr *)&connect->sockAddress, sizeof(struct sockaddr_in));
    if (rez < 0)
    {
        perror("P: bind failure");
        goto lExit;
    }

    rez = listen(connect->fd1, 0);
    if (rez < 0)
    {
        perror("P:listen failure");
        goto lExit;
    }

lExit:
    return rez;
}

// accept connection
// return 0 (success) or 1 (failure)
int acceptAndCommunication(struct stConnection *connect)
{
    int rez = EXIT_SUCCESS;
    pthread_t th;

    int fd2 = -1;
    socklen_t len = sizeof(connect->sockAddress);

    // make accept
    fd2 = accept(connect->fd1, (struct sockaddr *)&connect->sockAddress, &len);
    if (fd2 < 0) // error
    {
        perror("P: accept failure");
        rez = EXIT_FAILURE;
    }
    else // session started
    {
        // create gamer
        struct stGamer *gamer = (struct stGamer *)malloc(sizeof(struct stGamer));
        struct stCommunication *context = (struct stCommunication *)malloc(sizeof(struct stCommunication));
        if (!gamer || !context)
        {
            perror("malloc gamer or context failure");
            rez = EXIT_FAILURE;
            goto lExit;
        }
        memset(gamer, 0, sizeof(struct stGamer));
        memset(context, 0, sizeof(struct stCommunication));

        gamer->fd2 = fd2;
        gamer->IP = atoi(inet_ntoa(connect->sockAddress.sin_addr)); // TODO check that need htons
        context->gamer = gamer;
        context->connection = connect;

        // create thread of gamer for communication
        int phRez = pthread_create(&th, NULL, communication, (void *)context);
        if (phRez != 0) // error
        {
            perror("P: pthread_create failure");
            rez = EXIT_FAILURE;
            goto lExit;
        }
        printf("communication created : \nfd2 = %d \nIP = %d\n", gamer->fd2, gamer->IP);
    }

lExit:
    return rez;
}

// communication with client
void *communication(void *args)
{
    // // declaration & initialisation
    struct stCommunication *context = (struct stCommunication *)(args);
    //struct stGamer gamer = context->gamer;
    int fd2 = context->gamer->fd2;

    ssize_t rezRecv = -1;
    ssize_t rezSend = -1;

    //char pseudo[SIZE_ID];
    char bufer[BUF_SIZE];
    char answer[BUF_SIZE];

    processINFO(context, rezSend, answer);

    // listen answers
    while (1)
    {
        //rezRecv = recv(fd2, bufer, BUF_SIZE, 0);

        rezRecv = recieveMessage(fd2, bufer, TCP_END);
        if (rezRecv < 0) //error occured
        {
            perror("recieve client's request failure");
            goto lClose;
        }
        else if (0 == rezRecv) // client has disconnected
        {
            goto lClose;
        }

        char req[6]; // all messages start from 5 letters + white
        strncpy(req, bufer, 5);

        if (0 == strcmp(req, "NEWPL")) //-> NEWPL id port*** -> ask create game
        {
            processNEWPL(context, bufer, rezSend, answer);
        }

        else if (0 == strcmp(req, "REGIS"))
        {
            processREGIS(context, bufer, rezSend, answer);
        }

        else if (0 == strcmp(req, "UNREG"))
        {
            processUNREG(context, bufer, rezSend, answer);
        }

        else if (0 == strcmp(req, "SIZE?"))
        {
            processSIZE_(context, bufer, rezSend, answer);
        }

        else if (0 == strcmp(req, "LIST?"))
        {
            processLIST_(context, bufer, rezSend, answer);
        }

        else if (0 == strcmp(req, "GAME?"))
        {
            processGAME_(context, bufer, rezSend, answer);
        }

        else if (0 == strcmp(req, "GAMES"))
        {
            processGAMES(context, bufer, rezSend, answer);
        }

        //        - if START recieved from all gamers :                                             // TODO how manage this ? (routes, links between threads)
        //                      + create thread
        //                      + create UDP
        //                      + launch engine on it with this stGame
    }

lClose:
    //CLOSE(fd2);
    return NULL;
}

//////////////////////////// Process functions //////////////////////////////

// send info about all games and about each game
// return -1 if error, else 0
ssize_t processINFO(struct stCommunication *context, ssize_t rezSend, char *answer)
{
    int fd2 = context->gamer->fd2;
    unsigned int n = context->connection->games->count;
    //send info about all games
    sprintf(answer, "%s%d%s%c", GAMES, n, TCP_END, '\0');

    rezSend = send(fd2, answer, strlen(answer), 0);
    if (rezSend != strlen(answer))
    {
        perror("GAMES<n>*** sending failure");
        return -1;
    }

    //send info about each game
    struct element_t *gameEl = context->connection->games->first;
    struct stGame *game = NULL;
    while (gameEl)
    {
        game = (struct stGame *)(gameEl->data);

        sprintf(answer, "%s%d% d%s%c", OGAME, game->idGame, game->gamers->count, TCP_END, '\0');

        rezSend = send(fd2, answer, strlen(answer), 0);

        if (rezSend != strlen(answer))
        {
            printf("OGAME<m><s>*** for %d sending failure\n", game->gamers->count); //for debug
            perror("OGAME<m><s>*** for sending failure");
            return -1;
        }
        gameEl = gameEl->next;
    }
    return 0;
}

ssize_t processNEWPL(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer)
{    
    int fd2 = context->gamer->fd2;
    // create new game
    struct stGame *newGame = createGame(context, bufer);
    if (newGame) //REGOK m*** -> inscription OK
    {
        // add this game to list of games
        pushLast(context->connection->games, newGame);
        printf("lastGameId = %d\n", context->connection->lastGameId);

        // add name to this gamer
        char name[SIZE_ID];
        strncpy(name, bufer + 6, 8);
        name[8] = '\0';
        context->gamer->id = name;
        printf("context->gamer %s - %d - %d - %d - %d\n", context->gamer->id, context->gamer->IP, context->gamer->score, context->gamer->x, context->gamer->y);

        // add this gamer to list of gamers
        pushLast(newGame->gamers, context->gamer);

        //compose answer
        sprintf(answer, "%s%d%s%c", REGOK, newGame->idGame, TCP_END, '\0');
    }
    else // REGNO***
    {
        //compose answer
        sprintf(answer, "%s%s%c", REGNO, TCP_END, '\0');
    }

    rezSend = send(fd2, answer, strlen(answer), 0);
    if (rezSend < strlen(answer))
    {
        perror("REGOK / REGNO sending failure");
        return -1;
    }
    return 0;
}

ssize_t processREGIS(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer)
{    
    //-> REGIS id port m*** -> rejoindre partie num m
    // if ask join game :
    //        - add into list of gamers of this game
    //                REGOK m*** -> inscription OK
    //                REGNO*** -> inscription refusée
    return 0;
}

ssize_t processUNREG(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer)
{    
    //-> UNREG m*** -> se désinscrire d'une partie
    //        - remove from list of gamers of this game
    //                UNROK m** -> désinscription OK
    //                DUNNO*** -> m inconnu
    return 0;
}

ssize_t processSIZE_(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer)
{
    //-> SIZE? m*** -> dde taille du labyrinthe
    //               SIZE! m h w*** -> h hauteur, w largeur
    //               DUNNO*** -> m inconnu
    return 0;
}

ssize_t processLIST_(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer)
{
    //-> LIST? m*** -> liste des joueurs inscrits à une partie
    //                LIST! m s*** -> s = nb de joueurs,
    //                PLAYR id***-> s fois ce msg, id = id du joueur
    //                DUNNO***
    return 0;
}

ssize_t processGAME_(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer)
{
    //-> GAME?*** -> dde liste des parties non commencées
    //                LIST! m s*** -> s = nb de joueurs,
    //                PLAYR id***-> s fois ce msg, id = id du joueur
    //                DUNNO***
    return 0;
}

ssize_t processGAMES(struct stCommunication *context, char *bufer, ssize_t rezSend, char *answer)
{
    //-> GAMES n*** -> n = nb de parties
    //                OGAME m s***-> n fois ce msg
    return 0;
}

//////////////////////////// Auxiliary functions /////////////////////////////////

// create new game and fill folowing fields: idGame, port.
// return this new game
struct stGame *createGame(struct stCommunication *context, char *bufer)
{
    //create new stGame
    struct stGame *game = (struct stGame *)malloc(sizeof(struct stGame));
    if (!game)
    {
        perror("malloc stGame");
        return NULL;
    }
    game->gamers = (struct listElements_t *)malloc(sizeof(struct listElements_t));
    game->labirinth = (struct stLabirinth *)malloc(sizeof(struct stLabirinth));
    if (!game->gamers || !game->labirinth)
    {
        perror("malloc listElements_t in createGame");
        return NULL;
    }

    context->connection->lastGameId++;
    game->idGame = context->connection->lastGameId;

    //fill <port> field (bufer = "NEWPL id port***")
    char port[4];
    strncpy(port, bufer + 6 + 9, 4);
    game->port = atoi(port);

    return game;
}

// recieve till ending = "***" or "+++" will be recieved
// return -1 if error, 0 if session has been closed, else the number of bytes received
ssize_t recieveMessage(int fd, char *bufer, char *ending)
{
    ssize_t got = 0;
    ssize_t rezRecv = -1;
    const ssize_t szEnding = strlen(ending);

    while (got < BUF_SIZE)
    {
        rezRecv = recv(fd, bufer + got, BUF_SIZE - got, 0);
        if (rezRecv < 0) //error
        {
            // termine with error
            perror("recieve failure");
            return -1;
        }
        else if (rezRecv == 0) // server has disconnected
        {
            // termine this session
            printf("Client has close session!\n");
            return 0;
        }
        got += rezRecv;

        if (got > szEnding)
        {
            // check last 3 bytes before \0
            if (0 == strncmp(bufer + got - szEnding, ending, szEnding))
            {
                bufer[got] = 0;
                return got;
            }
        }
    }
    return -1;
}