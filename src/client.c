#include "server.h"
#include "commons.h"
#include "server_aux_functions.h" 
                 

// Usage: ./client REGIS <name> <port>

int main(int argc, char **argv)
{
    char answer[BUF_SIZE];
    char* ansIter = (char *)answer;

    char buf[BUF_SIZE];
    int rez = EXIT_SUCCESS;
    char* end = "***";

    char *keyWord = argv[1]; 
    char * name = argv[2];    
    char *port = argv[3];
    uint8_t m = 1;

    printf("keyWord %s - name %s - port %s - game %d\n", keyWord, name, port, m);

    int fd = 0;
    const char *adressString = "127.0.0.1";

    struct sockaddr_in socAddress;
    memset(&socAddress, 0, sizeof(struct sockaddr_in));

    //convert IP text addr to binary form
    socAddress.sin_family = AF_INET;
    socAddress.sin_port = htons(4242);
    socAddress.sin_addr.s_addr = inet_addr(adressString);

    //create a connection using IP add binary form and connect
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("cannot create socket");
        rez = EXIT_FAILURE;
        return rez;
    }

    //connect
    if (connect(fd, (struct sockaddr *)&socAddress, sizeof(struct sockaddr_in)))
    {
        perror("connection failure");
        rez = EXIT_FAILURE;
        CLOSE(fd);
        return rez;
    }

    recieveMessage(fd, buf, TCP_END);
    printf("%s\n", buf);

    if (0 == strcmp(keyWord, "NEWPL"))
    {
        // NEWPL id port***
    }

    else if (0 == strcmp(keyWord, "REGIS"))
    {
        // REGIS id port m***        
        
        char prefix[32];
        sprintf(prefix, "%s %s %s %c", keyWord, name, port, '\0');
        //printf("%s", prefix);
        printf("client has been send : %s", prefix);

        memcpy(ansIter, prefix, strlen(prefix));
        ansIter += strlen(prefix);    
        memcpy(ansIter, &m, sizeof(m));
        ansIter += sizeof(m);
        printf("%d", m);

        memcpy(ansIter, end, strlen(end));
        printf("%s\n", end);


        ssize_t rezSend = send(fd, answer, 24, 0);
        if (rezSend < 24)
        {
            perror("REGIS sending failure");
            rez = EXIT_FAILURE;
            CLOSE(fd);
            return rez;
        }
    }

    else if (0 == strcmp(keyWord, "UNREG"))
    {
    }

    else if (0 == strcmp(keyWord, "SIZE?"))
    {
    }

    else if (0 == strcmp(keyWord, "LIST?"))
    {
    }

    else if (0 == strcmp(keyWord, "GAME?"))
    {
    }

    else if (0 == strcmp(keyWord, "GAMES"))
    {
    }

    recieveMessage(fd, buf, TCP_END);
    printf("%s\n", buf);

    return 0;
}