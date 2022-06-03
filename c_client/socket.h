#ifndef SOCKET
#define SOCKET

enum eTcpDirection
{
    eTcpDirectionRead,
    eTcpDirectionWrite
};

int      tcp_connect(const char *pIp, uint16_t port);
void     tcp_disconnect(int socket);

bool     socket_isReady(int socket, enum eTcpDirection eDir, uint32_t timeOut);
bool     get_ip(int socket, char *pIp, size_t szIp);

int      udp_make_socket(const char *pIp, uint16_t maxPort, uint16_t *pPort);
int      udp_make_multicast_socket(uint32_t ip, uint16_t port);

#endif //SOCKET