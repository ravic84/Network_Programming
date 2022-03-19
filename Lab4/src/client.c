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


int Socket(int domain, int type, int protocol)
{
    int res = socket(domain, type, protocol);
    if (res == -1)
    {
        perror("CLIENT: Ошибка socket");
        exit(EXIT_FAILURE);
    }
    return res;
}
void Getsockname(int sockfd, struct sockaddr *addr, socklen_t *len)
{
    if (getsockname(sockfd, (struct sockaddr *)addr, len) < 0)
    {
        perror("CLIENT: Ошибка getsockname");
        exit(EXIT_FAILURE);
    }
}
void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int res = connect(sockfd, addr, addrlen);
    if (res == -1)
    {
        perror("CLIENT: Ошибка connect");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    int i, n;

    if (argc < 4)
    {
        printf("ENTER ./bin/client hostname port n\n");
        exit(1);
    }
    sock = Socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    bcopy(hp->h_addr, &server_addr.sin_addr, hp->h_length);
    server_addr.sin_port = htons(atoi(argv[2]));

    Connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    printf("CLIENT: Готов к отправке\n");
    printf("CLIENT: Запустил процесс пересылки\n");
    n = atoi(argv[3]);
    for (i = 1; i < n+1; i++)
    {
        send(sock, &i, sizeof(int), 0);
        printf("CLIENT: Отправка %d из %d\n", i, n);
        sleep(1);
    }

    printf("CLIENT: Пересылка завершена\n");
    close(sock);

    return 0;
}