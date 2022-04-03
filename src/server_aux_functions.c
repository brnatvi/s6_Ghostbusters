#include "server.h"
#include "commons.h"

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


// communication with client
void *communication(void *args)
{   
    // // declaration & initialisation
    struct stCommunication *context = (struct stCommunication *)(args);
    //struct stGamer gamer = context->gamer;
    int fd2 = context->gamer->fd2;   
    unsigned int n = context->connection->games->count;

    ssize_t rezRecv = -1;
    ssize_t rezSend = -1;

    //char pseudo[SIZE_ID];
    char bufer[BUF_SIZE];
    char answer[BUF_SIZE];

    //send info about all games
    sprintf(answer, "%s%d%s%c", GAMES, n, TCP_END, '\0');

    rezSend = send(fd2, answer, strlen(answer), 0);
    if (rezSend != strlen(answer))
    {
        perror("GAMES<n>*** sending failure");
        goto lClose;
    }

    //send info about each game
    struct element_t *gameEl = context->connection->games->first;
    struct stGame *game = NULL;
    while (gameEl)
    {
        game = (struct stGame *)(gameEl->data);

        sprintf(answer, "%s%d%d%s%c", OGAME, game->idGame, game->gamers->count, TCP_END, '\0');

        rezSend = send(fd2, answer, strlen(answer), 0);

        if (rezSend != strlen(answer))
        {
            printf("OGAME<m><s>*** for %d sending failure\n", game->gamers->count); //for debug
            perror("OGAME<m><s>*** for sending failure");
            goto lClose;
        }
        gameEl = gameEl->next;
    }  

    // listen answers
    while (1)
    {
        rezRecv = recv(fd2, bufer, BUF_SIZE, 0);
        if (rezRecv < 0) //error occured
        {
            perror("recieve client's request failure");
            goto lClose;
        }
        else if (0 == rezRecv) // client has disconnected
        {
            goto lClose;
        }

        char req[6];
        strncpy(req, bufer, 5); // all messages start from 5 letters

        if (0 == strcmp(req, "NEWPL"))
        {
            printf("%s", bufer);
            //-> NEWPL id port*** -> ask create game
            //        - create new stGame, add it into list of games
            //        - add this gamer into list of gamers
            //                REGOK m*** -> inscription OK
            //                REGNO*** -> inscription refusée
        }
        else if (0 == strcmp(req, "REGIS"))
        {
            printf("%s", bufer);
            //-> REGIS id port m*** -> rejoindre partie num m
            // if ask join game :
            //        - add into list of gamers of this game
            //                REGOK m*** -> inscription OK
            //                REGNO*** -> inscription refusée
        }
        else if (0 == strcmp(req, "UNREG"))
        {
            printf("%s", bufer);
            //-> UNREG m*** -> se désinscrire d'une partie
            //        - remove from list of gamers of this game
            //                UNROK m** -> désinscription OK
            //                DUNNO*** -> m inconnu
        }
        else if (0 == strcmp(req, "SIZE?"))
        {
            //-> SIZE? m*** -> dde taille du labyrinthe
            //               SIZE! m h w*** -> h hauteur, w largeur
            //               DUNNO*** -> m inconnu
        }
        else if (0 == strcmp(req, "LIST?"))
        {
            //-> LIST? m*** -> liste des joueurs inscrits à une partie
            //                LIST! m s*** -> s = nb de joueurs,
            //                PLAYR id***-> s fois ce msg, id = id du joueur
            //                DUNNO***
        }
        else if (0 == strcmp(req, "GAME?"))
        {
            //-> GAME?*** -> dde liste des parties non commencées
            //                LIST! m s*** -> s = nb de joueurs,
            //                PLAYR id***-> s fois ce msg, id = id du joueur
            //                DUNNO***
        }
        else if (0 == strcmp(req, "GAMES"))
        {
            //-> GAMES n*** -> n = nb de parties
            //                OGAME m s***-> n fois ce msg
        }

        //        - if START recieved from all gamers :                                             // TODO how manage this ? (routes, links between threads)
        //                      + create thread
        //                      + create UDP
        //                      + launch engine on it with this stGame
    }

lClose:
    CLOSE(fd2);
    return NULL;
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
        goto lExit;
    }
    else // session started
    {
        // create gamer
        struct stGamer *gamer = (struct stGamer *)malloc(sizeof(struct stGamer));
        memset(gamer, 0, sizeof(struct stGamer));
        gamer->fd2 = fd2;
        gamer->IP = atoi(inet_ntoa(connect->sockAddress.sin_addr)); // TODO check that need htons

        struct stCommunication *context = (struct stCommunication *)malloc(sizeof(struct stCommunication));
        memset(context, 0, sizeof(struct stCommunication));
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
    //return rez;                                                                 // TODO check if need return here

lExit:
    return rez;
}
