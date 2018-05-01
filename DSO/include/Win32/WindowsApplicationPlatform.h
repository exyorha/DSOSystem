#ifndef WIN32__WINDOWS_APPLICATION_PLATFORM__H
#define WIN32__WINDOWS_APPLICATION_PLATFORM__H

#include <GUI/ApplicationPlatform.h>
#include <Win32/WindowsHostWindow.h>

class WindowsApplicationPlatform final : public ApplicationPlatform {
public:
    WindowsApplicationPlatform();
    virtual ~WindowsApplicationPlatform();

    virtual DisplayInformation displayInformation() const override;
    virtual void requestFrame() override;
    virtual void keepPreviousFrame() override;

    virtual void setApplicationPlatformInterface(ApplicationPlatformInterface *iface) override;

    virtual void exec() override;

private:
    CAtlWinModule m_winModule;
    WindowsHostWindow m_hostWindow;
};

#endif
