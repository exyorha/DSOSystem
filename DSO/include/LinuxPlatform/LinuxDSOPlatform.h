#ifndef LINUX_PLATFORM__LINUX_DSO_PLATFORM__H
#define LINUX_PLATFORM__LINUX_DSO_PLATFORM__H

#include <DSO/DSOPlatform.h>
#include <LinuxPlatform/LinuxEventLoop.h>

class LinuxDSOPlatform final : public DSOPlatform {
public:
    LinuxDSOPlatform();
    virtual ~LinuxDSOPlatform();
    
    virtual std::unique_ptr<ApplicationPlatform> createGUIPlatform() override;
    virtual std::unique_ptr<DSOAcquisition> createAcquisition() override;

    inline LinuxEventLoop &eventLoop() { return m_eventLoop; }

private:
    LinuxEventLoop m_eventLoop;
};

#endif
