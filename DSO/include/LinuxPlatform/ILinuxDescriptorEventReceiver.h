#ifndef LINUX_PLATFORM__I_LINUX_DESCRIPTOR_EVENT_RECEIVER__H
#define LINUX_PLATFORM__I_LINUX_DESCRIPTOR_EVENT_RECEIVER__H

class ILinuxDescriptorEventReceiver {
protected:
    inline ILinuxDescriptorEventReceiver() { }
    inline ~ILinuxDescriptorEventReceiver() { }
    
public:
    virtual void handleEvents(unsigned int events) = 0;
};

#endif