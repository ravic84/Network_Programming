#include <arpa/inet.h>
#include <memory.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>



#define MAXIMUM 4
#define BUF_SIZE 1024


typedef struct
{
    int sockClient;
    struct sockaddr_in clientAddr;
    int fd;
} PthreadData;

pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t MUTEXFILE = PTHREAD_MUTEX_INITIALIZER;

void Write(int fd, char *buf, int len)
{
    int res = write(fd, buf, len);
    if (res == -1)
    {
        perror("SERVER: Ошибка write");
        exit(EXIT_FAILURE);
    }
}
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
void *work(void *inputData)
{
    int n,fd,status, sockClient;
    char *buf = malloc(BUF_SIZE * sizeof(char));;
    struct sockaddr_in clientAddr;

    PthreadData *data = (PthreadData *)inputData;
    sockClient = data->sockClient;
    clientAddr = data->clientAddr;
    fd = data->fd;
    pthread_mutex_unlock(&MUTEX);

    while (1)
    {
        if ((recv(sockClient, &n, sizeof(int), 0)) <= 0)
        {
            break;
        }
        printf("SERVER: Адрес клиента: %s:%d, ",
                inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
                printf("Полученное число - %d\n", n);
                //printf("Поток - %d\n", pthread_self());
        pthread_mutex_lock(&MUTEXFILE);

        status = sprintf(buf, "%s %s%c%d%s %s %d\n", "IP/port", inet_ntoa(clientAddr.sin_addr), '/', ntohs(clientAddr.sin_port),",", "value -", n);
        if (status < 0)
        {
            perror("SERVER: Ошибка sprintf");
            exit(EXIT_FAILURE);
        }
        Write(fd, buf, strlen(buf));

        pthread_mutex_unlock(&MUTEXFILE);
    }
    close(sockClient);
    pthread_exit(NULL);
}
int File_fd()
{
    int fd;
    fd = open("bin/data.txt", O_CREAT | O_EXCL | O_WRONLY, S_IRWXU);
    if (fd == -1)
    {
        fd = open("bin/data.txt", O_WRONLY | O_TRUNC, S_IRWXU);
        if (fd == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
    }
    return fd;
}
int main()
{
    int sockClient, sockMain, status, fd;
    socklen_t length;
    struct sockaddr_in servAddr, clientAddr;
    pthread_t thread;
    PthreadData data;

    fd = File_fd();

    sockMain = Socket(AF_INET, SOCK_STREAM, 0);
    memset(&servAddr, 0, sizeof(servAddr));

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
        pthread_mutex_lock(&MUTEX);

        data.sockClient = sockClient;
        data.clientAddr = clientAddr;
        data.fd = fd;

        status = pthread_create(&thread, NULL, work, &data);
        if (status != 0)
        {
            perror("SERVER: Ошибка pthread_create");
            exit(EXIT_FAILURE);
        }
        status = pthread_detach(thread);
        if (status != 0)
        {
            perror("SERVER: Ошибка pthread_detach");
            exit(EXIT_FAILURE);
        }
    }
    close(fd); 
    close(sockMain);
    return 0;
}