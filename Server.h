#pragma once

#include <string>
#include <map>
#include <memory>
#include "Reactor.h"
#include "Handler.h"

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

    int port_;
    std::string ip_;
    std::unique_ptr<Reactor> base_;
    std::map<int, std::shared_ptr<Handler>> handleList_;
};