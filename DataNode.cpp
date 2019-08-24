#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <vector>
#include "./DataNode.h"
#include "./tools.h"
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
    while(true)
    {
        int nbytes = read(sock_fd_, cmd, 1024);
        cout<<cmd<<endl;
        if(nbytes > 0 )
        {
            cmd[nbytes] = '\0';
            cout<<cmd<<endl;
        }
        vector<string> buff;
        split(cmd, buff, ' ');
        memset(cmd, 0, sizeof(cmd));
        
        // 解析命令
        if(buff[0] == "GET")
        {
            string value = cache_.get(buff[1]);
            strcpy(cmd, value.c_str());
        }
        else if(buff[0] == "PUT")
        {
            cache_.put(buff[1], buff[2]);
            strcpy(cmd, "SUCCESS");
        }

        write(sock_fd_, cmd, strlen(cmd));
    }
}

string DataNode::Get(string key)
{

}

void DataNode::Put(string key, string value)
{

}


