#pragma once

#include "EpollPoller.h"
#include <memory>

class Handler;

class Reactor
{
public:
    Reactor();
    void registerHandler(Handler*);
    void removeHandler(Handler*);
    void loop();
private:
    std::unique_ptr<EpollPoller> poller_;
};