#ifndef FREEBSD_PLATFORM__FREEBSD_APPLICATION_PLATFORM__H
#define FREEBSD_PLATFORM__FREEBSD_APPLICATION_PLATFORM__H

#include <GUI/ApplicationPlatform.h>
#include <FreeBSDPlatform/FreeBSDVTRenderer.h>

class FreeBSDDSOPlatform;

class FreeBSDApplicationPlatform final : public ApplicationPlatform {
public:
    FreeBSDApplicationPlatform(FreeBSDDSOPlatform *platform);
    virtual ~FreeBSDApplicationPlatform();

    virtual DisplayInformation displayInformation() const override;
    virtual void requestFrame() override;
    virtual void keepPreviousFrame() override;

    virtual void setApplicationPlatformInterface(ApplicationPlatformInterface *iface) override;

    virtual void exec() override;

    virtual void postDefer() override;

private:
    FreeBSDDSOPlatform *m_platform;
    ApplicationPlatformInterface *m_gui;
    FreeBSDVTRenderer m_renderer;
    bool m_frameRequested;
};

#endif
