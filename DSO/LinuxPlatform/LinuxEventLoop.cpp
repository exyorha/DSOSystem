#include <LinuxPlatform/LinuxEventLoop.h>
#include <LinuxPlatform/LinuxError.h>
#include <LinuxPlatform/ILinuxDescriptorEventReceiver.h>

LinuxEventLoop::LinuxEventLoop() : m_stopOnEmpty(false), m_fdsRegistered(0) {    
    int fd = epoll_create1(EPOLL_CLOEXEC);
    if(fd < 0) {
        throw LinuxError();
    }
    
    m_epoll.reset(fd);
}

LinuxEventLoop::~LinuxEventLoop() {

}

void LinuxEventLoop::registerDescriptor(int fd, ILinuxDescriptorEventReceiver *receiver, unsigned int events) {
    struct epoll_event event;
    event.events = events;
    event.data.ptr = receiver;
    
    if(epoll_ctl(m_epoll, EPOLL_CTL_ADD, fd, &event) < 0)
        throw LinuxError();

    m_fdsRegistered++;
}

void LinuxEventLoop::updateDescriptor(int fd, ILinuxDescriptorEventReceiver *receiver, unsigned int events) {
    struct epoll_event event;
    event.events = events;
    event.data.ptr = receiver;
    
    if(epoll_ctl(m_epoll, EPOLL_CTL_MOD, fd, &event) < 0)
        throw LinuxError();
}

void LinuxEventLoop::unregisterDescriptor(int fd) {
    if(epoll_ctl(m_epoll, EPOLL_CTL_DEL, fd, nullptr) < 0)
        throw LinuxError();

    m_fdsRegistered--;
}
    
void LinuxEventLoop::dispatch() {
    while(!m_stopOnEmpty || m_fdsRegistered != 0) {
        while(!m_deferredQueue.empty()) {
            m_deferredQueue.front()();
            m_deferredQueue.pop_front();
        }

        int result = epoll_wait(m_epoll, m_epollEvents.data(), m_epollEvents.size(), -1);
        if(result < 0) {
            if(errno == EINTR)
                continue;
                
            throw LinuxError();
        }
        
        for(int desc = 0; desc < result; desc++) {
            static_cast<ILinuxDescriptorEventReceiver *>(m_epollEvents[desc].data.ptr)->handleEvents(m_epollEvents[desc].events);
        }
    }
}

void LinuxEventLoop::defer(std::function<void()> &&deferred) {
    m_deferredQueue.emplace_back(deferred);
}
