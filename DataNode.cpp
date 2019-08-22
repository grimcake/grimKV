#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include "./DataNode.h"
#include <iostream>
using namespace std;

DataNode::DataNode(string server_ip, int server_port, int id) :
    server_ip_(server_ip),
    server_port_(server_port),
    id_(id)
{
}

DataNode::~DataNode()
{

}

void DataNode::start()
{
    startup();
    sendDataInfo();
    cout<<"here"<<endl;
    dealServerCmd();
}

void DataNode::startup()
{
    struct sockaddr_in servaddr;
    sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    inet_pton(AF_INET, server_ip_.c_str(), &servaddr.sin_addr);
    connect(sock_fd_, (struct sockaddr *) &servaddr, sizeof(servaddr));
}

void DataNode::sendDataInfo()
{
    string info = "DATANODE "+to_string(id_);
    write(sock_fd_, info.c_str(), info.size());
}

void DataNode::sendHeartInfo()
{

}

void DataNode::dealServerCmd()
{
    char cmd[20];
    memset(cmd, 0, sizeof(cmd));
    while(true)
    {
        int nbytes = read(sock_fd_, cmd, 1024);
        if(nbytes > 0 )
        {
            cmd[nbytes] = '\0';
            cout<<cmd<<endl;
        }
        memset(cmd, 0, sizeof(cmd));
        //strcpy(cmd, "fuck you");
        //write(sock_fd_, cmd, 2);
    }
}

