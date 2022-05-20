#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <time.h>

#include "socket.h"

#if !defined(NULL_DESCRIPTOR )
    #define NULL_DESCRIPTOR -1
#endif    

int tcp_connect(const char *pIp, uint16_t port)
{
    int ret = NULL_DESCRIPTOR;
    struct sockaddr_in socAddress;
    memset(&socAddress, 0, sizeof(struct sockaddr_in));

    socAddress.sin_family      = AF_INET;
    socAddress.sin_port        = htons(port);
    socAddress.sin_addr.s_addr = inet_addr(pIp);

    ret = socket(AF_INET, SOCK_STREAM, 0);

    if (NULL_DESCRIPTOR == ret)
    {
        return NULL_DESCRIPTOR;
    }

    if (connect(ret, (struct sockaddr *)&socAddress, sizeof(struct sockaddr_in)))
    {
        close(ret);
        ret = NULL_DESCRIPTOR;
    }
    
    return ret;
}

    //multi-cast
    //https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6
    //http://www.iana.org/assignments/multicast-addresses/multicast-addresses.xhtml
    //https://stackoverflow.com/questions/236231/how-do-i-choose-a-multicast-address-for-my-applications-use
    //https://man.openbsd.org/getsockname.2


void tcp_disconnect(int iSocket)
{
    if (NULL_DESCRIPTOR != iSocket) { return; }
    int32_t l_iFlag = 0;
    setsockopt(iSocket, SOL_SOCKET, SO_LINGER, (char*)&l_iFlag, sizeof(l_iFlag));
    shutdown(iSocket, SHUT_RDWR);
    close(iSocket);
}

bool socket_isReady(int iSocket, enum eTcpDirection eDir, uint32_t i_dwTimeOut)
{
    bool l_bReturn = false;

    if (NULL_DESCRIPTOR == iSocket)
    {
        return false;
    }

    struct timeval stTimeOut;
    fd_set         stFDS;

    stTimeOut.tv_sec  = 0;
    stTimeOut.tv_usec = i_dwTimeOut * 1000; 

    FD_ZERO(&stFDS);
    FD_SET((uint32_t)iSocket, &stFDS);

    int iSelect = NULL_DESCRIPTOR;

    if (eTcpDirectionRead == eDir)
    {
        iSelect = select(((uint32_t)iSocket) + 1,
                         &stFDS,
                         NULL, 
                         NULL,
                         &stTimeOut
                         );
    }
    else 
    {
        iSelect = select(((uint32_t)iSocket) + 1, 
                         NULL, 
                         &stFDS, 
                         NULL, 
                         &stTimeOut
                         );
    }

    if ( (iSelect > 0) && (FD_ISSET(iSocket, &stFDS)) )
    {
        l_bReturn = true;
    }

    return l_bReturn;
}


bool get_ip(int iSocket, char *pIp, size_t szIp)
{
    if (iSocket == NULL_DESCRIPTOR)
    {
        return false;
    }
    
    struct sockaddr_storage stSas;
    socklen_t sLen = sizeof(stSas);
    if (0 != getsockname(iSocket, (struct sockaddr*)&stSas, &sLen))
    {
        return false;
    }

    if (AF_INET != stSas.ss_family) //IPV4 only
    {
        return false;
    }

    if (NULL == inet_ntop(stSas.ss_family, 
                         &((struct sockaddr_in*)&stSas)->sin_addr,
                          pIp,
                          szIp
                         )
        )
    {
        return false;
    }

    return true;
}


int udp_make_socket(const char *pIp, uint16_t uMaxPort, uint16_t *pPort)
{
    if ((!pIp) || (!pPort))
    {
        return NULL_DESCRIPTOR;
    }

    int iSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (iSocket <= 0)
    {
        return NULL_DESCRIPTOR;
    }

    struct sockaddr_in socAddress;
    memset(&socAddress, 0, sizeof(struct sockaddr_in));

    socAddress.sin_family      = AF_INET;
    socAddress.sin_addr.s_addr = inet_addr(pIp);

    while (1)
    {
        // find free port
        *pPort = rand() % (uMaxPort - 1025); 
        socAddress.sin_port = htons(*pPort);
        if (0 == bind(iSocket, (struct sockaddr *)&socAddress, sizeof(socAddress)))
        {
            break;
        }
    }

    return iSocket;
}


int udp_make_multicast_socket(uint32_t uIp, uint16_t uPort)
{
    int iSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (iSocket < 0) 
    {
        return -1;
    }

    uint32_t yes = 1;
    if (setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)) < 0)
    {
        close(iSocket);
       return -1;
    }    

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    addr.sin_port = htons(uPort);

    if (bind(iSocket, (struct sockaddr*) &addr, sizeof(addr)) < 0) 
    {
        close(iSocket);
        return -1;
    }    

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = htonl(uIp);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(iSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) < 0)
    {
        close(iSocket);
        return -1;
    }
    return iSocket;
}
