#pragma once

#include "Callback.h"

#define MAX_BUFFLEN 1024

class Handler
{
public:
    Handler(int fd);
    ~Handler();
    void handleEvent();
    void setReadCallback(EventCallback cb)
    {
        readCallback_ = cb;
    }
    void setWriteCallback(EventCallback cb)
    {
        writeCallback_ = cb;
    }
    void setCloseCallback(EventCallback cb)
    {
        closeCallback_ = cb;
    }
    void enableRead();
    void enableWrite();
    void enableAll();
    void disableAll();
    int fd() const { return fd_; }
    int events() const { return events_; }
    void setEvents(int op) {
        events_ = op;
    }
    int revents() const { return revents_; }
    void setRevents(int op) {
        revents_ = op;
    }
    char* buff() { return buff_; }
    void setLen(int len) { len_ = len;}
    int len() const { return len_; }   
    bool isInEpoll() const { return isInEpoll_; }
    void setIsInEpoll(bool flag) {
        isInEpoll_ = flag;
    }
private:
    int fd_;
    int events_;
    int revents_;
    bool isInEpoll_;
    char buff_[MAX_BUFFLEN];
    int len_;
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};