#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <functional>
#include <string.h>
#include "Server.h"
#include "tools.h"

using namespace std::placeholders;

Server::Server(int port) :
    port_(port),
    base_(new Reactor),
    consHash_(0, 0)
{

}

void Server::start()
{
    startup();
    base_->loop();
}

void Server::startup()
{
    int msockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(msockfd < 0)
    {
        printf("socket error\n");
        exit(-1);
    }
    auto mHandler = std::make_shared<Handler>(msockfd);
    handleList_[msockfd] = mHandler;

    int reuse = -1;
    setsockopt(mHandler->fd(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);
    sockaddr_in addr;
    bzero(&addr, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = ::bind(mHandler->fd(), (sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        printf("bind error\n");
    }

    ret = listen(mHandler->fd(), 16);
    if(ret < 0)
    {
        printf("listen error\n");
    }

    mHandler->setReadCallback(std::bind(&Server::acceptConn, this, _1));
    mHandler->enableRead();
    base_->registerHandler(mHandler.get());
}

void Server::acceptConn(int fd)
{
    int acceptfd = accept4(fd, NULL, NULL, SOCK_NONBLOCK);
    if(acceptfd < 0)
    {
        printf("accept error\n");
    }
    auto connHandler = std::make_shared<Handler>(acceptfd);
    handleList_[acceptfd] = connHandler;
    connHandler->setReadCallback(std::bind(&Server::readData, this, _1));
    connHandler->enableRead();
    base_->registerHandler(connHandler.get());
}

void Server::readData(int fd)
{
    auto pHandler = handleList_[fd];
    int nbytes = 0;
    nbytes = read(fd, pHandler->buff(), MAX_BUFFLEN);

    if(nbytes > 0)
    {
        pHandler->buff()[nbytes] = 0;
        printf(" %s\n", pHandler->buff());
        // 解析命令
        vector<string> cmd;
        split(pHandler->buff(), cmd, ' ');
        if(cmd[0] == "DATANODE")
        {
            // 添加新的数据节点
            addToDataServer(atoi(cmd[1].c_str()), fd);
        }
        else if(cmd[0] == "HEART")
        {
            // 接受心跳信息
            //pHandler->setWriteCallback(std::bind(&Server::readData, this, _1));
            //pHandler->enableRead();
            //base_->registerHandler(pHandler.get());
        }
        else if(cmd[0] == "PUT")
        {
            // 计算key所在的的dataserver，将写请求转发给dataserver
            int data_server_index = consHash_.getServerIndex(cmd[1]);
            int data_server_fd = dataServerList_[data_server_index];
            dataToClient_[data_server_fd] = fd;
            cout<<pHandler->buff()<<endl;
            //write(data_server_fd, pHandler->buff(), nbytes);

            clientToData_[fd] = data_server_fd;
            pHandler->setLen(nbytes);
            pHandler->setWriteCallback(std::bind(&Server::sendToDataServer, this, _1));
            pHandler->enableWrite();
            base_->registerHandler(pHandler.get());
        }
        else if(cmd[0] == "GET")
        {
            // 计算key所在的dataserver，将读请求转发给dataserver
            int data_server_index = consHash_.getServerIndex(cmd[1]);
            int data_server_fd = dataServerList_[data_server_index];
            dataToClient_[data_server_fd] = fd;
            cout<<pHandler->buff()<<endl;
            //write(data_server_fd, pHandler->buff(), nbytes);

            clientToData_[fd] = data_server_fd;
            pHandler->setLen(nbytes);
            pHandler->setWriteCallback(std::bind(&Server::sendToDataServer, this, _1));
            pHandler->enableWrite();
            base_->registerHandler(pHandler.get());
        }
        else {
            // 由dataserver返回的结果
            /*
            printf("%s\n", pHandler->buff());
            pHandler->setLen(nbytes);
            pHandler->setWriteCallback(std::bind(&Server::sendData, this, _1));
            pHandler->enableWrite();
            base_->registerHandler(pHandler.get());*/
            write(dataToClient_[fd], pHandler->buff(), nbytes);
        }
    }
    else if(nbytes == 0)
    {
        printf("close fd\n");
        ::close(fd);
        base_->removeHandler(pHandler.get());
        handleList_.erase(fd);
    }
    else
    {
        ::close(fd);
        printf("read error\n");
        base_->removeHandler(pHandler.get());
        handleList_.erase(fd);
    }
    
}



void Server::sendToDataServer(int fd)
{
    auto pHandler = handleList_[fd];
    int nbytes = write(clientToData_[fd], pHandler->buff(), pHandler->len());
    if(nbytes > 0)
    {
        pHandler->setReadCallback(std::bind(&Server::readData, this, _1));
        pHandler->enableRead();
        base_->registerHandler(pHandler.get());
    }
    else
    {
        printf("write error\n");
        ::close(fd);
        base_->removeHandler(pHandler.get());
        handleList_.erase(fd);
    }
}


void Server::sendData(int fd)
{
    auto pHandler = handleList_[fd];
    int nbytes = write(fd, pHandler->buff(), pHandler->len());
    if(nbytes > 0)
    {
        pHandler->setReadCallback(std::bind(&Server::readData, this, _1));
        pHandler->enableRead();
        base_->registerHandler(pHandler.get());
    }
    else
    {
        printf("write error\n");
        ::close(fd);
        base_->removeHandler(pHandler.get());
        handleList_.erase(fd);
    }
    
}

void Server::addToDataServer(int index, int fd)
{
    consHash_.addNewNode(index);
    dataServerList_[index] = fd;
}