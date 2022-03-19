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

void Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    if (select(FD_SETSIZE, readfds, NULL, NULL, NULL) < 0)
    {
        perror("SERVER: Ошибка select");
        exit(EXIT_FAILURE);
    }
}

int receive_data(int fd, struct sockaddr_in clientAddr)
{
    int bytes_read, n;
    bytes_read = recv(fd, &n, sizeof(int), 0);
    if (bytes_read < 0)
    {
        perror("SERVER: Ошибка receive_data");
        exit(EXIT_FAILURE);
    }
    else if (bytes_read == 0)
    {
        return 1;
    }
    printf("SERVER: Адрес клиента: %s:%d, ",
                inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
                printf("Полученное число - %d\n", n);
    return 0;
}

int main()
{
    int sockClient, sockMain,fd;
    socklen_t length;
    struct sockaddr_in server_addr, tmp_addr, clientAddr[FD_SETSIZE];
    fd_set readfds, activefds;

    sockMain = Socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = 0;

    Bind(sockMain, (struct sockaddr *)&server_addr, sizeof(server_addr)); 
    length = sizeof(server_addr);
    Getsockname(sockMain, (struct sockaddr *)&server_addr, &length);
    printf("SERVER: PORT - %d\n", ntohs(server_addr.sin_port));
    
    Listen(sockMain, MAXIMUM);
    FD_ZERO(&activefds);
    FD_SET(sockMain, &activefds);
    while (1)
    {
        memcpy(&readfds, &activefds, sizeof(readfds));
        Select(FD_SETSIZE, &readfds, NULL, NULL, NULL); 
        if (FD_ISSET(sockMain, &readfds))
        {
            sockClient = Accept(sockMain, (struct sockaddr *)&tmp_addr, &length);
            memcpy(&clientAddr[sockClient], &tmp_addr, sizeof(clientAddr));
            FD_SET(sockClient, &activefds);
        }

        for (fd = 0; fd < FD_SETSIZE; fd++)
        {
            if (fd != sockMain && FD_ISSET(fd, &readfds))
            {
                if (receive_data(fd, clientAddr[fd]))
                {
                    close(fd);
                    FD_CLR(fd, &activefds);
                    memset(&clientAddr[fd], 0, sizeof(sizeof(struct sockaddr_in)));
                }
            }
        }
    }
    close(sockMain);
    return 0;
}