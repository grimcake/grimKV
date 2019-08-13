#include "Reactor.h"
#include "EpollPoller.h"
#include "Handler.h"


Reactor::Reactor() :
    poller_(new EpollPoller)
{
    
}

void Reactor::registerHandler(Handler* handler)
{
    poller_->update(handler);
}

void Reactor::removeHandler(Handler* handler)
{
    poller_->remove(handler);
}

void Reactor::loop()
{
    while(true)
    {
        auto activeEventPtrList = poller_->poll();
        if(!activeEventPtrList.empty())
        {
            for(int i = 0; i<activeEventPtrList.size(); i++)
            {
                activeEventPtrList[i]->handleEvent();
            }
        }
        else
        {        
        }
        
    }
}