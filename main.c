#include "common.h"
#include <stdio.h>


int main(int argc, char **argv)
{
    struct stContext stContext;
    memset(&stContext, 0, sizeof(stContext));
    
    setTerminalTextColor(DEFAULT_TERM_COLOR);

    stContext.cliW = getTerminalWidth();
    stContext.cliH = getTerminalHeight();
    
    const char *pIp = "127.0.0.1";
    if ((argc != 3) || (0 == strcmp(argv[1], "--help")))
    {
        printf("Ghostbusters game, welcome!\nRun:\n");      
        printf("> ./ghost_client <server hostname> <player id>\n");
        printf("> Example: ./ghost_client locomotive Pernata1\n");
        printf("> player_id - hase to be 8 characters long\n");
        return 0;
    }

    pIp = argv[1];
    if (strlen(argv[2]) != USER_ID_LEN)
    {
        printf("ERROR: player_id hase to be 8 characters long\n");
        return 0;
    }
    strcpy(stContext.userId, argv[2]);

    stContext.tcpSocket     = tcp_connect(pIp, SERVER_PORT);
    stContext.udpPeerSocket = NULL_DESCRIPTOR;

    if (NULL_DESCRIPTOR == stContext.tcpSocket)
    {
        log_error("Can't connect to server %s:%u\n", pIp, (uint32_t)SERVER_PORT);
        goto labelExit;
    }
    else
    {
        char pPeerIp[32];
        get_ip(stContext.tcpSocket, pPeerIp, sizeof(pPeerIp));
        stContext.udpPeerSocket = udp_make_socket(pPeerIp, 9999, &stContext.udpPeerPort);
    }

    if (NULL_DESCRIPTOR == stContext.udpPeerSocket)
    {
        log_error("Can't create UDP socket\n");
        goto labelExit;
    }

    stContext.stTcpBuf.szWritten = 0;
    stContext.stUdpBuf.szWritten = 0;
    stContext.gamesCount         = 0;
    stContext.pGames             = NULL;
    stContext.pSelectedGame      = NULL;
    stContext.bExit              = false;

    updateGamesInfo(&stContext, false);
    printGamesInfo(&stContext);
    startGamesMenu(&stContext);

labelExit:
    if (NULL_DESCRIPTOR != stContext.udpPeerSocket)
    {
        close(stContext.udpPeerSocket);
    }

    freeGames(&stContext);
    freeGameSet(&stContext);

    tcp_disconnect(stContext.tcpSocket);
    return 0;
}
