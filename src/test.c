#include "server.h"
#include "commons.h"
#include "server_aux_functions.h"
#include <fcntl.h>

// Usage:
//          ./test NEWPL <name> <port>
//          ./test REGIS <name> <port>
//          ./test UNREG <name>
//          ./test LIST?
//          ./test GAME?

static uint32_t uGameId = 0;

int receiveMessages(int iSocket)
{
    struct stRawMessage stMsg;
    struct stIpBuffer   stTcpBuf;

    uint32_t uExpectedMsg = 1;
    while (uExpectedMsg)
    {
        stMsg.msg = eUdpMsgMaxValue;
        int iMsgRes = tcpGetMsg(iSocket, &stTcpBuf, &stMsg);

        if (iMsgRes < 0)
        {
            fprintf(stderr, "{%s} Socket/Communication error, close connection!\n", __FUNCTION__);
            return -1;
        }
        else if (iMsgRes == 0)
        {
            continue;
        }

        uExpectedMsg--;
        if (stMsg.msg == eTcpMsgGAMES)
        {
            if (1 != scanMsg(stMsg.msgRaw, eTcpMsgGAMES, &uExpectedMsg))
            {
                fprintf(stderr, "{%s} Unexpected message eTcpMsgGAMES!\n", __FUNCTION__);
                return -1;
            }

            printf(">>Server is reprorting %u games\n", uExpectedMsg);
        }
        else if (stMsg.msg == eTcpMsgOGAME)
        {
            uint32_t m,s;
            if (2 != scanMsg(stMsg.msgRaw, eTcpMsgOGAME, &m, &s))
            {
                fprintf(stderr, "{%s} Unexpected message eTcpMsgOGAME!\n", __FUNCTION__);
                return -1;
            }
            printf(">> * Game %u with %u players\n", m, s);
        }
        else if (stMsg.msg == eTcpMsgREGOK)
        {
            uint32_t m;
            if (1 != scanMsg(stMsg.msgRaw, eTcpMsgREGOK, &m))
            {
                fprintf(stderr, "{%s} Unexpected message eTcpMsgREGOK!\n", __FUNCTION__);
                return -1;
            }
            printf(">>REGOK %u\n", m);
            uGameId = m;
        }
        else if (stMsg.msg == eTcpMsgREGNO)
        {
            printf(">>REGNO\n");
        }
        else if (stMsg.msg == eTcpMsgDUNNO)
        {
            printf(">>DUNNO\n");
        }
        else if (stMsg.msg == eTcpMsgLISTA)
        {
            uint32_t m;
            uint32_t s;
            if (2 != scanMsg(stMsg.msgRaw, eTcpMsgLISTA, &m, &s))
            {
                fprintf(stderr, "{%s} Unexpected message eTcpMsgLISTA!\n", __FUNCTION__);
                return -1;
            }
            printf(">>LIST! game ID:%u players: %u\n", m, s);
            uExpectedMsg = s;
        }
        else if (stMsg.msg == eTcpMsgPLAYR)
        {
            char pId[USER_ID_LEN];
            if (1 != scanMsg(stMsg.msgRaw, eTcpMsgPLAYR, pId))
            {
                fprintf(stderr, "{%s} Unexpected message eTcpMsgPLAYR!\n", __FUNCTION__);
                return -1;
            }
            printf(">>PlAYR ID:%s\n", pId);
        }
    }

    return 0;
}


int main(int argc, char **argv)
{   
    struct sockaddr_in  socAddress;
    int                 fd = 0;
    
    memset(&socAddress, 0, sizeof(struct sockaddr_in));
    //convert IP text addr to binary form
    socAddress.sin_family      = AF_INET;
    socAddress.sin_port        = htons(4242);
    socAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    //create a connection using IP add binary form and connect
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("cannot create socket");
        return 1;
    }

    //connect
    if (connect(fd, (struct sockaddr *)&socAddress, sizeof(struct sockaddr_in)))
    {
        perror("connection failure");
        CLOSE(fd);
        return 1;
    }

    argc--; argv++; //skip process name

    if (0 != receiveMessages(fd))
    {
        CLOSE(fd);
        return 1;
    }

    while (argc--)
    {
        const struct stMsgDesc *pMsgDesc = getMsgDescriptionByPrefix(*argv);
        if (!pMsgDesc)
        {
            fprintf(stderr, "{%s} Unknown message %s\n", __FUNCTION__, *argv);
            break;
        }

        argv++;

        if (pMsgDesc->msg == eTcpMsgNEWPL)
        {
            if (argc < 2) {fprintf(stderr, "{%s} arguments ID/port are missing\n", __FUNCTION__); break; }
            const char *pId   = *argv; argv++; argc--;
            int32_t     iPort = atoi(*argv); argv++; argc--;
            
            printf("Send command %s %s %d\n", pMsgDesc->prefix, pId, iPort);
            if (!sendMsg(fd, eTcpMsgNEWPL, pId, (uint32_t)iPort))
            {
                fprintf(stderr, "{%s} eTcpMsgNEWPL sending failure!", __FUNCTION__);
                break;
            }
            if (0 != receiveMessages(fd)) { break; }
        }
        else if (pMsgDesc->msg == eTcpMsgREGIS)
        {
            if (argc < 3) {fprintf(stderr, "{%s} arguments ID/port/m are missing\n", __FUNCTION__); break; }
            const char *pId   = *argv; argv++; argc--;
            int32_t     iPort = atoi(*argv); argv++; argc--;
            int32_t     iM    = atoi(*argv); argv++; argc--;

            printf("Send command %s %s %d %d\n", pMsgDesc->prefix, pId, iPort, iM);

            if (!sendMsg(fd, eTcpMsgREGIS, pId, (uint32_t)iPort, (uint32_t)iM))
            {
                fprintf(stderr, "{%s} eTcpMsgREGIS sending failure!", __FUNCTION__);
                break;
            }
            if (0 != receiveMessages(fd)) { break; }
        }
        else if (pMsgDesc->msg == eTcpMsgUNREG)
        {
            printf("Send command %s\n", pMsgDesc->prefix);

            if (!sendMsg(fd, eTcpMsgUNREG))
            {
                fprintf(stderr, "{%s} eTcpMsgUNREG sending failure!", __FUNCTION__);
                break;
            }
            if (0 != receiveMessages(fd)) { break; }
        }
        else if (pMsgDesc->msg == eTcpMsgLISTQ)
        {
            if (argc < 1) {fprintf(stderr, "{%s} argument m are missing\n", __FUNCTION__); break; }
            int32_t iM = atoi(*argv); argv++; argc--;

            printf("Send command %s %d\n", pMsgDesc->prefix, iM);

            if (!sendMsg(fd, eTcpMsgLISTQ, (uint32_t)iM))
            {
                fprintf(stderr, "{%s} eTcpMsgLISTQ sending failure!", __FUNCTION__);
                break;
            }
            if (0 != receiveMessages(fd)) { break; }
        }
        else if (pMsgDesc->msg == eTcpMsgGAMEQ)
        {
            printf("Send command %s\n", pMsgDesc->prefix);

            if (!sendMsg(fd, eTcpMsgGAMEQ))
            {
                fprintf(stderr, "{%s} eTcpMsgGAMEQ sending failure!", __FUNCTION__);
                break;
            }
            if (0 != receiveMessages(fd)) { break; }
        }
    }
            
    shutdown(fd, SHUT_RDWR);
    CLOSE(fd);
    return 0;
}