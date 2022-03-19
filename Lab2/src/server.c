#include <arpa/inet.h>
#include <memory.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define MAXIMUM 10


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
void Listen(int sockfd, int backlog)
{
    int res = listen(sockfd, backlog);
    if (res == -1)
    {
        perror("SERVER: Ошибка listen");
        exit(EXIT_FAILURE);
    }
}
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int res = accept(sockfd, addr, addrlen);
    if (res == -1)
    {
        perror("SERVER: Ошибка accept");
        exit(EXIT_FAILURE);
    }
    return res;
}

int main()
{
    signal(SIGCHLD, SIG_IGN); // ps -ax | grep defunct
    int sockClient, sockMain;
    socklen_t length;
    struct sockaddr_in servAddr, clientAddr;
    int n;
    pid_t PID;

    sockMain = Socket(AF_INET, SOCK_STREAM, 0);
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = 0;
    Bind(sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr));

    length = sizeof(servAddr);
    Getsockname(sockMain, (struct sockaddr *)&servAddr, &length);
    printf("SERVER: PORT - %d\n", ntohs(servAddr.sin_port));
    Listen(sockMain, MAXIMUM);

    while (1)
    {
        sockClient = Accept(sockMain, (struct sockaddr *)&clientAddr, &length);
        PID = fork();
        switch (PID)
        {
        case 0:
            close(sockMain);
            while (1)
            {
                if ((recv(sockClient, &n, sizeof(int), 0)) <= 0)
                {
                    break;
                }
                printf("SERVER: Адрес клиента: %s:%d, ",
                inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
                printf("Полученное число - %d\n", n);
            }
            close(sockClient);
            exit(EXIT_SUCCESS);
            break;
        case -1:
            perror("SERVER: FORK");
            exit(EXIT_SUCCESS);
            break;
        default:
            close(sockClient);
            break;
        }
    }
    close(sockMain);
    return 0;
}