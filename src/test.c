#include "server.h"
#include "commons.h"
#include "server_aux_functions.h"
#include <fcntl.h>

//#define PRINT_PROTOCOL

// Usage:
//          ./test NEWPL <name> <port>
//          ./test REGIS <name> <port>
//          ./test UNREG <name>
//          ./test LIST?
//          ./test GAME?

int main(int argc, char **argv)
{   

    char answer[BUF_SIZE];
    char *iter = (char *)answer;

    char buf[BUF_SIZE];
    int rez = EXIT_SUCCESS;

    char *keyWord = argv[1];
    char *name = argv[2];
    char *port = argv[3];
    uint8_t m = 1;                      // sequence number of game TO CHANGE

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

    ssize_t len = recieveMessage(fd, buf, TCP_END);

#if defined(PRINT_PROTOCOL)
    iter = buf;
    for (ssize_t i = 0; i < len; i++)
    {
        printf("0x%02X(%c) ", *iter, *iter ? *iter : '0');
        iter++;
    }
    printf("\n");
#endif
    int CCCCC = 1;

    if (0 == strcmp(keyWord, NEWPL))
    {
        // NEWPL id port***

        sprintf(answer, "%s %s %s%s%c", NEWPL, name, port, TCP_END, '\0');
        printf("client has been send : %s\n", answer);
        //fflush(stdout);

        ssize_t rezSend = send(fd, answer, strlen(answer), 0);
        if (rezSend < strlen(answer))
        {
            perror("NEWPL sending failure");
            rez = EXIT_FAILURE;
            CLOSE(fd);
            return rez;
        }
    }

    else if (0 == strcmp(keyWord, REGIS))
    {
        // REGIS id port m***

        char prefix[32];
        sprintf(prefix, "%s %s %s %c", REGIS, name, port, '\0');
        iter = answer;

        memcpy(iter, prefix, strlen(prefix));
        iter += strlen(prefix);

        memcpy(iter, &m, sizeof(m));
        iter += sizeof(m);

        memcpy(iter, TCP_END, strlen(TCP_END));
        iter += strlen(TCP_END);

        ssize_t answerLen = iter - answer;

        ssize_t rezSend = send(fd, answer, answerLen, 0);
        if (rezSend < answerLen)
        {
            perror("REGIS sending failure");
            rez = EXIT_FAILURE;
            CLOSE(fd);
            return rez;
        }
    }

    else if (0 == strcmp(keyWord, UNREG))
    {
        // UNREG m***
        iter = answer;

        memcpy(iter, keyWord, strlen(keyWord));
        iter += strlen(keyWord);

        memcpy(iter, WHITE, strlen(WHITE));
        iter += strlen(WHITE);

        memcpy(iter, &m, sizeof(m));
        iter += sizeof(m);

        memcpy(iter, TCP_END, strlen(TCP_END));
        iter += strlen(TCP_END);

        ssize_t answerLen = iter - answer;

        ssize_t rezSend = send(fd, answer, answerLen, 0);
        if (rezSend < answerLen)
        {
            perror("UNREG sending failure");
            rez = EXIT_FAILURE;
            CLOSE(fd);
            return rez;
        }
    }

    else if (0 == strcmp(keyWord, "SIZE?"))
    {
    }

    else if (0 == strcmp(keyWord, LIST_Q))
    {
        CCCCC = 5;
        // LIST? m***
        iter = answer;

        memcpy(iter, keyWord, strlen(keyWord));
        iter += strlen(keyWord);

        memcpy(iter, WHITE, strlen(WHITE));
        iter += strlen(WHITE);

        memcpy(iter, &m, sizeof(m));
        iter += sizeof(m);

        memcpy(iter, TCP_END, strlen(TCP_END));
        iter += strlen(TCP_END);

        ssize_t answerLen = iter - answer;

        ssize_t rezSend = send(fd, answer, answerLen, 0);
        if (rezSend < answerLen)
        {
            perror("LIST? sending failure");
            rez = EXIT_FAILURE;
            CLOSE(fd);
            return rez;
        }
    }

    else if (0 == strcmp(keyWord, GAME_Q))
    {
        // GAME?***
        sprintf(answer, "%s%s%c", keyWord, TCP_END, '\0');

        ssize_t rezSend = send(fd, answer, strlen(answer), 0);
        if (rezSend < strlen(answer))
        {
            perror("GAME? sending failure");
            rez = EXIT_FAILURE;
            CLOSE(fd);
            return rez;
        }
    }

    
    for (int i =0; i < CCCCC; i++)
    {
        printf("MSG %d\n", i);
        recieveMessage(fd, buf, TCP_END);   
    }

    CLOSE(fd);
    return 0;
}