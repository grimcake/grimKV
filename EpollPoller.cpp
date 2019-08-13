#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>
#include "EpollPoller.h"
#include "Handler.h"

EpollPoller::EpollPoller() : activeEventList_(kInitEventListSize) //初始化vector
{
    epollfd_ = epoll_create1(EPOLL_CLOEXEC);
}

EpollPoller::~EpollPoller()
{
    close(epollfd_);
}

void EpollPoller::update(Handler *handler)
{
    epoll_event epollEvent;
    epollEvent.events = handler->events();
    epollEvent.data.ptr = handler;
    int op;
    if (handler->isInEpoll())
    {
        op = EPOLL_CTL_MOD;
    }
    else
    {
        op = EPOLL_CTL_ADD;
    }
    handler->setIsInEpoll(true);
    int ret = epoll_ctl(epollfd_, op, handler->fd(), &epollEvent);
    if (ret < 0)
    {
        printf("epoll_ctl error on fd = %d\n", handler->fd());
    }
    else
    {
        {
            printf("epoll_ctl on fd = %d success\n", handler->fd());
        }
    }
}

void EpollPoller::remove(Handler *handler)
{
    assert(handler->isInEpoll());
    handler->setIsInEpoll(false);
    int op = EPOLL_CTL_DEL;
    int ret = epoll_ctl(epollfd_, op, handler->fd(), NULL);
    if (ret < 0)
    {
        printf("epoll_ctl error on fd = %d\n", handler->fd());
    }
    handler->setIsInEpoll(false);
}

std::vector<Handler *>
EpollPoller::fillActiveChannels(int activeNums)
{
    std::vector<Handler *> activeEventPtrList;
    for (int i = 0; i < activeNums; i++)
    {
        Handler *handlerPtr = static_cast<Handler *>(activeEventList_[i].data.ptr);
        if (activeEventList_[i].events & EPOLLIN)
        {
            handlerPtr->setRevents(EPOLLIN);
        }
        if (activeEventList_[i].events & EPOLLOUT)
        {
            handlerPtr->setRevents(EPOLLOUT);
        }
        activeEventPtrList.push_back(handlerPtr);
    }
    return activeEventPtrList;
}

std::vector<Handler *>
EpollPoller::poll()
{
    std::vector<Handler *> activeEventPtrList;
    int nreadys = epoll_wait(epollfd_, activeEventList_.data(), activeEventList_.size(), -1);
    if (nreadys > 0)
    {
        activeEventPtrList = fillActiveChannels(nreadys);
        if (nreadys == activeEventList_.size())
        {
            activeEventList_.resize(activeEventList_.size() * 2);
        }
    }
    else if (nreadys == 0)
    {
    }
    else
    {
        printf("nreadys = %d, error\n", nreadys);
    }
    return activeEventPtrList;
}
