#ifndef GUI__APPLICATION_PLATFORM__H
#define GUI__APPLICATION_PLATFORM__H

#include <GUI/DisplayInformation.h>
#include <SkRefCnt.h>

#include <functional>

class SkSurface;

class ApplicationPlatformInterface;

class ApplicationPlatform {
protected:
    ApplicationPlatform();

public:
    virtual ~ApplicationPlatform();

    virtual DisplayInformation displayInformation() const = 0;
    virtual void requestFrame() = 0;
    virtual void keepPreviousFrame() = 0;

    virtual void setApplicationPlatformInterface(ApplicationPlatformInterface *iface) = 0;

    virtual void exec() = 0;

	virtual void postDefer() = 0;
};

#endif
