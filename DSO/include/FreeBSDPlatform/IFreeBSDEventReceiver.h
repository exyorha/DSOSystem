#ifndef FREEBSD_PLATFORM__I_FREEBSD_EVENT_RECEIVER__H
#define FREEBSD_PLATFORM__I_FREEBSD_EVENT_RECEIVER__H

struct kevent;

class IFreeBSDEventReceiver {
protected:
    inline IFreeBSDEventReceiver() { }
    inline ~IFreeBSDEventReceiver() { }

public:
    virtual void handleEvent(const struct kevent &event) = 0;
};

#endif
