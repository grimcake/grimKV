#pragma once

#include <string>
#include <map>
#include <memory>
#include "Reactor.h"
#include "Handler.h"
#include "DataNode.h"
#include "ConsistentHash.h"
using namespace std;

class Reactor;

class Server
{
public:
    Server(int port);
    void start();
    void startup();
private:
    void acceptConn(int fd);
    void readData(int fd);
    void sendData(int fd);
    // index:新节点的编号，fd:新节点的连接描述符
    void addToDataServer(int index, int fd);

    int port_;
    std::string ip_;
    std::unique_ptr<Reactor> base_;
    std::map<int, std::shared_ptr<Handler>> handleList_;
    map<int, int> dataServerList_; // key是dataserver id，value是fd
    ConsistentHash consHash_; // 一致性hash
};