#ifndef LINUX_PLATFORM__LINUX_EVENT_LOOP__H
#define LINUX_PLATFORM__LINUX_EVENT_LOOP__H

#include "LinuxManagedDescriptor.h"

#include <sys/epoll.h>
#include <array>
#include <list>
#include <functional>

class ILinuxDescriptorEventReceiver;

class LinuxEventLoop {
public:
    LinuxEventLoop();
    ~LinuxEventLoop();
    
    void registerDescriptor(int fd, ILinuxDescriptorEventReceiver *receiver, unsigned int events);
    void updateDescriptor(int fd, ILinuxDescriptorEventReceiver *receiver, unsigned int events);
    void unregisterDescriptor(int fd);
    
    void dispatch();
    
    inline bool stopOnEmpty() const { return m_stopOnEmpty; }
    inline void setStopOnEmpty(bool stopOnEmpty) { m_stopOnEmpty = stopOnEmpty; }

    void defer(std::function<void()> &&deferred);

private:
    LinuxManagedDescriptor m_epoll;
    std::array<struct epoll_event, 1> m_epollEvents;
    bool m_stopOnEmpty;
    unsigned int m_fdsRegistered;
    std::list<std::function<void()>> m_deferredQueue;
};

#endif