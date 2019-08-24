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
    void sendToDataServer(int fd);

    int port_;
    std::string ip_;
    std::unique_ptr<Reactor> base_;
    std::map<int, std::shared_ptr<Handler>> handleList_;
    map<int, int> dataServerList_; // key是dataserver id，value是fd
    ConsistentHash consHash_; // 一致性hash
    // 由于共用一个poll，因此在client发起PUT或GET请求时，需要记录所对应的dataserver->client的映射
    // 之后接收到dataserver发来的信息后就向key对应的value发送结果
    map<int, int> dataToClient_; 
    map<int, int> clientToData_;
};