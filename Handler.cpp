#include <sys/epoll.h>
#include "Handler.h"

Handler::Handler(int fd) :
    fd_(fd),
    events_(0),
    revents_(0),
    isInEpoll_(false),
    len_(0)
{

}

Handler::~Handler()
{

}

void Handler::handleEvent()
{
    if(revents_ & EPOLLIN)
    {
        readCallback_(fd_);
    }
    if(revents_ & EPOLLOUT)
    {
        writeCallback_(fd_);
    }
}

void Handler::enableRead()
{
    events_ = (EPOLLIN);
}

void Handler::enableWrite()
{
    events_ = (EPOLLOUT);
}

void Handler::enableAll()
{
    events_ |= EPOLLIN;
    events_ |= EPOLLOUT;
}

void Handler::disableAll()
{
    events_ = 0;
}