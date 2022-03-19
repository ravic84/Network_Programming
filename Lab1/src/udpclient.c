#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>



#define BUF_SIZE 256

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
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int res = bind(sockfd, addr, addrlen);
    if (res == -1)
    {
        perror("CLIENT: Ошибка bind");
        exit(EXIT_FAILURE);
    }
}
void Getsockname(int sockfd, struct sockaddr *addr, socklen_t *len)
{
    if (getsockname(sockfd, (struct sockaddr *)addr, len) < 0)
    {
        perror("CLIENT: Ошибка getsockname");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{ 
    int sock;
    struct sockaddr_in servAddr, clientAddr ;
    struct hostent *hp;
    
    if(argc < 3)
    { 
        printf( "CLIENT: Введите ./bin/udpclient hostname port value\n"); 
        exit(EXIT_FAILURE);
    }
    
    sock = Socket( AF_INET, SOCK_DGRAM, 0 );
    bzero( (char *) &servAddr, sizeof(servAddr) ) ;

    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    bcopy(hp->h_addr, &servAddr.sin_addr, hp->h_length);
    servAddr.sin_port = htons(atoi(argv[2]));

    memset(&clientAddr, 0, sizeof(clientAddr));

    //bzero( (char *)&clientAddr, sizeof(clientAddr ) ) ;
    clientAddr.sin_family = AF_INET ; 
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY) ;
    clientAddr.sin_port = 0 ;
    Bind( sock, (struct sockaddr *)&clientAddr, sizeof(clientAddr) ) ;

    printf("CLIENT: Готов к пересылке.\n");

    int n = atoi(argv[3]);
    for (int i = 1; i < n + 1; i++)
    { 
        if (sendto(sock,  &i, sizeof(int), 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
        {
            perror("CLIENT: Ошибка отправки");
            exit(3);
        }
        printf("CLIENT: Отправка %d из %d\n", i, n);
        sleep(1);
    }

    /*char *buf = malloc(sizeof(char) * BUF_SIZE);
    memset(buf, 0, sizeof(char) * BUF_SIZE);
    strcpy(buf, argv[3]);
    if (sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("sendto");
        exit(3);
    }*/

    printf("CLIENT: Пересылка завершена. \n"); 
    close(sock);
    return 0; 
}
