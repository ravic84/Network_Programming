#include <arpa/inet.h>
#include <memory.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>

#define BUF_SIZE 1024

int Socket(int domain, int type, int protocol)
{
    int res = socket(domain, type, protocol);
    if (res == -1)
    {
        perror("SERVER: Ошибка socket");
        exit(EXIT_FAILURE);
    }
    return res;
}
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int res = bind(sockfd, addr, addrlen);
    if (res == -1)
    {
        perror("SERVER: Ошибка bind");
        exit(EXIT_FAILURE);
    }
}
void Getsockname(int sockfd, struct sockaddr *addr, socklen_t *len)
{
    if (getsockname(sockfd, (struct sockaddr *)addr, len) < 0)
    {
        perror("SERVER: Ошибка getsockname");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    int sockMain, msglength;
    socklen_t length;
    struct sockaddr_in servAddr, clientAddr;
    
    sockMain = Socket(AF_INET, SOCK_DGRAM, 0);
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = 0;
    Bind(sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr));

    length = sizeof(servAddr);
    Getsockname(sockMain, (struct sockaddr *)&servAddr, &length);
    printf("SERVER: PORT - %d\n", ntohs(servAddr.sin_port));


    int n;
    while (1)
    {
        if ( (msglength = recvfrom( sockMain, &n, sizeof(int), 0 , (struct sockaddr *)&clientAddr, &length) ) < 0 )
        {
            perror("SERVER: Ошибка recvfrom");
            exit(EXIT_FAILURE);
        }
        printf("SERVER: Client address: %s:%d, ", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        printf("Получено число %d\n", n);
    }

    /*char *buf = malloc(sizeof(char) * BUF_SIZE);
     while (1)
    {
        memset(buf, 0, sizeof(char) * BUF_SIZE);
        if ( (msglength = recvfrom( sockMain, buf, BUF_SIZE, 0 , (struct sockaddr *)&clientAddr, &length) ) < 0 )
        {
            perror("SERVER: Ошибка recvfrom");
            exit(EXIT_FAILURE);
        }
        printf("SERVER: Client address: %s:%d, ", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        printf("Получено сообщение %s\n", buf);
    }*/

    close(sockMain);
    return 0;
}