#ifndef GUI__APPLICATION_PLATFORM_INTERFACE__H
#define GUI__APPLICATION_PLATFORM_INTERFACE__H

#include <stdint.h>
#include <pixman.h>

class ApplicationPlatformInterface {
protected:
    inline ApplicationPlatformInterface() { }
    inline ~ApplicationPlatformInterface() { }

public:
    ApplicationPlatformInterface(const ApplicationPlatformInterface &other) = delete;
    ApplicationPlatformInterface &operator =(const ApplicationPlatformInterface &other) = delete;

    virtual void drawFrame(pixman_image_t *display) = 0;

    virtual void keyPressed(uint32_t key) = 0;
    virtual void keyReleased(uint32_t key) = 0;

	virtual void dispatchDeferred() = 0;
};

#endif