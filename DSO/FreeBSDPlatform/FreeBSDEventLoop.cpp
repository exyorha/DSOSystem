#include <FreeBSDPlatform/FreeBSDEventLoop.h>
#include <FreeBSDPlatform/FreeBSDError.h>
#include <FreeBSDPlatform/IFreeBSDEventReceiver.h>

FreeBSDEventLoop::FreeBSDEventLoop() {
    int fd = kqueue();
    if(fd < 0) {
        throw FreeBSDError();
    }

    m_kqueue.reset(fd);
}

FreeBSDEventLoop::~FreeBSDEventLoop() = default;

void FreeBSDEventLoop::modifyEvent(struct kevent &&event, IFreeBSDDescriptorEventReceiver *receiver) {
  event.udata = receiver;

  int result = kevent(m_kqueue, &event, 1, nullptr, 0, nullptr);
  if(result < 0) {
    throw FreeBSDError();
  }
}

void FreeBSDEventLoop::modifyEvent(struct kevent &&event) {
  int result = kevent(m_kqueue, &event, 1, nullptr, 0, nullptr);
  if(result < 0) {
    throw FreeBSDError();
  }
}

void FreeBSDEventLoop::dispatch() {
    while(true) {
      while(!m_deferredQueue.empty()) {
          m_deferredQueue.front()();
          m_deferredQueue.pop_front();
      }

      struct kevent event;

      auto result = kevent(m_kqueue, nullptr, 0, &event, 1, nullptr);
      if(result < 0) {
        if(errno == EINTR)
          continue;

        throw FreeBSDError();
      }

      if(result > 0) {
        static_cast<IFreeBSDEventReceiver *>(event.udata)->handleEvent(event);
      }
  }
}

void FreeBSDEventLoop::defer(std::function<void()> &&deferred) {
    m_deferredQueue.emplace_back(std::move(deferred));
}
