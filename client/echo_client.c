#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERV_PORT 8888
#define MAXLINE  1024

void str_cli(FILE *fp, int sockfd);
// 这个readline代码很低效，因为它是一个字符一个字符的读取的
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;
    ptr = vptr;
    for (n = 1; n < maxlen; n++){
    again:
        if ( (rc = read(fd, &c, 1)) == 1){
            *ptr++ = c;
            if (c == '\n')
                break;  // newline is stored
        } else if (rc == 0){
            *ptr = 0;
            return (n - 1); // EOF, n - 1 bytes
        } else {
            if (errno == EINTR)
                    goto again;
            return -1; /// error, errno set by read()
        }
    }
    *ptr = 0;
    return n;
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2){
        fprintf(stderr, "usage: tcpcli <IPaddress>");
        return 1;
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    str_cli(stdin, sockfd);  // do it all
    exit(0);
}

void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    while (fgets(sendline, MAXLINE, fp) != NULL){
        write(sockfd, sendline, strlen(sendline));
        if (readline(sockfd, recvline, MAXLINE) == 0)
            printf("str_cli:server terminated prematurely");
        fputs(recvline, stdout);
    }
}
