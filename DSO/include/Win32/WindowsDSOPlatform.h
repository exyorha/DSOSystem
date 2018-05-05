#ifndef WIN32__WINDOWS_DSO_PLATFORM__H
#define WIN32__WINDOWS_DSO_PLATFORM__H

#include <DSO/DSOPlatform.h>
#include <Win32/WindowsLogSink.h>

class WindowsDSOPlatform final : public DSOPlatform {
public:
    WindowsDSOPlatform();
    virtual ~WindowsDSOPlatform();

    virtual std::unique_ptr<ApplicationPlatform> createGUIPlatform() override;
	virtual std::unique_ptr<DSOAcquisition> createAcquisition() override;

private:
	WindowsLogSink m_logSink;
};

#endif

