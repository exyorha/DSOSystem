#ifndef FREEBSD_PLATFORM__FREEBSD_EVENT_LOOP__H
#define FREEBSD_PLATFORM__FREEBSD_EVENT_LOOP__H

#include "FreeBSDManagedDescriptor.h"

#include <sys/event.h>
#include <array>
#include <list>
#include <functional>

class IFreeBSDDescriptorEventReceiver;

class FreeBSDEventLoop {
public:
    FreeBSDEventLoop();
    ~FreeBSDEventLoop();

    void modifyEvent(struct kevent &&event, IFreeBSDDescriptorEventReceiver *receiver);
    void modifyEvent(struct kevent &&event);

    void dispatch();

    void defer(std::function<void()> &&deferred);

private:
    FreeBSDManagedDescriptor m_kqueue;
    std::list<std::function<void()>> m_deferredQueue;
};

#endif
