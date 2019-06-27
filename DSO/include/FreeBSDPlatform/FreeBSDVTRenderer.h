#ifndef FREEBSD_PLATFORM_FREEBSD_DRM_RENDERER__H
#define FREEBSD_PLATFORM_FREEBSD_DRM_RENDERER__H

#include <FreeBSDPlatform/FreeBSDManagedDescriptor.h>

#include <GUI/DisplayInformation.h>
#include <GUI/PixmanUtils.h>

#include <vector>

class ApplicationPlatformInterface;

class FreeBSDVTRenderer {
public:
    FreeBSDVTRenderer(const char *deviceName);
    ~FreeBSDVTRenderer();

    FreeBSDVTRenderer(const FreeBSDVTRenderer &other) = delete;
    FreeBSDVTRenderer &operator =(const FreeBSDVTRenderer &other) = delete;

    inline const DisplayInformation &displayInfo() const { return m_displayInfo; }

    inline ApplicationPlatformInterface *gui() const { return m_gui; }
    inline void setGui(ApplicationPlatformInterface *gui) { m_gui = gui; }

    void drawFrame();
    void deliverDeferred();

private:
    struct Mapping {
        Mapping();
        ~Mapping();

        Mapping(const Mapping &other) = delete;
        Mapping &operator =(const Mapping &other) = delete;

        void *base;
        size_t size;
    };

    FreeBSDManagedDescriptor m_vt;
    DisplayInformation m_displayInfo;
    ApplicationPlatformInterface *m_gui;
    Mapping m_mapping;
    PixmanImage m_display;
};

#endif
