#ifndef FREEBSD_PLATFORM__FREEBSD_DSO_PLATFORM__H
#define FREEBSD_PLATFORM__FREEBSD_DSO_PLATFORM__H

#include <DSO/DSOPlatform.h>
#include <FreeBSDPlatform/FreeBSDConsoleLogSink.h>
#include <FreeBSDPlatform/FreeBSDEventLoop.h>

class FreeBSDDSOPlatform final : public DSOPlatform {
public:
    FreeBSDDSOPlatform();
    virtual ~FreeBSDDSOPlatform();

    virtual std::unique_ptr<ApplicationPlatform> createGUIPlatform() override;
    virtual std::unique_ptr<DSOAcquisition> createAcquisition() override;

    inline FreeBSDEventLoop &eventLoop() { return m_eventLoop; }

private:
    FreeBSDConsoleLogSink m_logSink;
    FreeBSDEventLoop m_eventLoop;
};

#endif
