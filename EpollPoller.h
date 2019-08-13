#pragma once

#include <vector>
#include <sys/epoll.h>
#include <memory>

class Handler;

class EpollPoller
{
public:
    EpollPoller();
    ~EpollPoller();
    void update(Handler*);
    void remove(Handler*);
    std::vector<Handler*> poll();
    static const int kInitEventListSize = 16;
private:
    typedef std::vector<epoll_event> EventList;
    std::vector<Handler*> fillActiveChannels(int activeNums);
    int epollfd_;
    EventList activeEventList_;
};