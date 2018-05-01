#ifndef LINUX_PLATFORM_LINUX_DRM_RENDERER__H
#define LINUX_PLATFORM_LINUX_DRM_RENDERER__H

#include <LinuxPlatform/LinuxManagedDescriptor.h>
#include <LinuxPlatform/ILinuxDescriptorEventReceiver.h>
#include <GUI/DisplayInformation.h>

#include <vector>

#include <xf86drm.h>
#include <SkRefCnt.h>

class LinuxEventLoop;
class SkSurface;
class ApplicationPlatformInterface;

class LinuxDRMRenderer final : private ILinuxDescriptorEventReceiver {
public:
    LinuxDRMRenderer(const char *deviceName, LinuxEventLoop &eventLoop);
    ~LinuxDRMRenderer();

    LinuxDRMRenderer(const LinuxDRMRenderer &other) = delete;
    LinuxDRMRenderer &operator =(const LinuxDRMRenderer &other) = delete;

    inline const DisplayInformation &displayInfo() const { return m_displayInfo; }

    inline ApplicationPlatformInterface *gui() const { return m_gui; }
    inline void setGui(ApplicationPlatformInterface *gui) { m_gui = gui; }

    void keepPreviousFrame();

private:
    virtual void handleEvents(unsigned int events) override;

    static void pageFlipHandlerThunk(int fd, unsigned int sequence, unsigned int tv_sec, unsigned int tv_usec, unsigned int crtc_id, void *user_data);
    void pageFlipHandler();

    struct ScreenBuffer {
        ScreenBuffer();
        ~ScreenBuffer();

        ScreenBuffer(const ScreenBuffer &other) = delete;
        ScreenBuffer &operator =(const ScreenBuffer &other) = delete;

        ScreenBuffer(ScreenBuffer &&other);
        ScreenBuffer &operator =(ScreenBuffer &&other);

        uint32_t width;
        uint32_t height;
        uint32_t handle;
        uint32_t pitch;
        uint64_t size;
        void *mapping;
        uint32_t fbObject;
        sk_sp<SkSurface> surface;
    };


    LinuxEventLoop &m_eventLoop;
    LinuxManagedDescriptor m_drm;
    DisplayInformation m_displayInfo;
    std::vector<ScreenBuffer> m_screenBuffers;
    uint32_t m_crtc;
    size_t m_readPointer, m_writePointer, m_buffersUsed;
    bool m_swapPending;
    ApplicationPlatformInterface *m_gui;

    static const drmEventContext m_drmEventContext;
};

#endif
