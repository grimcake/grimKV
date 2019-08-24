#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string>
#include <iostream>
using namespace std;
#define SERV_PORT 8888
#define MAXLINE  1024

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
    //str_cli(stdin, sockfd);  // do it all


    int i;
    for(i = 0; i<10; i++) {
        string keyi = "key"+to_string(i);
        string valuei = "value"+to_string(i);
        string cmd("GET "+keyi);
        int nbytes = write(sockfd, cmd.c_str(), cmd.size());
        if(nbytes<=0){
            cout<<"write error:"<<nbytes<<endl;
        }
        char buff[20];
        nbytes = read(sockfd, buff, 1024);
        if(nbytes<=0) {
            cout<<"read error:"<<nbytes<<endl;
        }
        else cout<<buff<<endl;
        sleep(3);
    }
    exit(0);
}

