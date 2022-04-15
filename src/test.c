#include "server.h"
#include "commons.h"
#include "server_aux_functions.h"
#include <fcntl.h>

#define PRINT_PROTOCOL



// Usage:
//          ./test NEWPL <name> <port>
//          ./test REGIS <name> <port>
//          ./test UNREG <name>

//void to_File(int fd, char *buf)
//{
//    int file = open("/home/nata/Documents/L3_Reseaux/1_Projet/test.bin", O_RDWR, O_APPEND,
//                    S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH);
//    if (file < 0)
//    {
//        perror("open failure");
//        CLOSE(fd);
//    }
//    int r = write(file, buf, strlen(buf));
//    if (r < strlen(buf))
//    {
//        perror("write failure");
//    }
//    CLOSE(file);
//}

int main(int argc, char **argv)
{   

    char answer[BUF_SIZE];
    char *iter = (char *)answer;

    char buf[BUF_SIZE];
    int rez = EXIT_SUCCESS;

    char *keyWord = argv[1];
    char *name = argv[2];
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

    CLOSE(fd);
    return 0;
}