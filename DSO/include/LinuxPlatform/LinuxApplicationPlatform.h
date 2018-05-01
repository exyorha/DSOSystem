#ifndef LINUX_PLATFORM__LINUX_APPLICATION_PLATFORM__H
#define LINUX_PLATFORM__LINUX_APPLICATION_PLATFORM__H

#include <GUI/ApplicationPlatform.h>
#include <LinuxPlatform/LinuxDRMRenderer.h>

class LinuxDSOPlatform;

class LinuxApplicationPlatform final : public ApplicationPlatform {
public:
    LinuxApplicationPlatform(LinuxDSOPlatform *platform);
    virtual ~LinuxApplicationPlatform();

    virtual DisplayInformation displayInformation() const override;
    virtual void requestFrame() override;
    virtual void keepPreviousFrame() override;

    virtual void setApplicationPlatformInterface(ApplicationPlatformInterface *iface) override;

    virtual void exec() override;

private:
    LinuxDSOPlatform *m_platform;
    ApplicationPlatformInterface *m_gui;
    LinuxDRMRenderer m_renderer;
};

#endif
