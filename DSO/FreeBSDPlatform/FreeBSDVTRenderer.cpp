#include <FreeBSDPlatform/FreeBSDVTRenderer.h>
#include <FreeBSDPlatform/FreeBSDError.h>
#include <FreeBSDPlatform/FreeBSDEventLoop.h>

#include <GUI/ApplicationPlatformInterface.h>

#include <Logging/LogFacility.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/fbio.h>

#include <memory>

LogFacility LogFreeBSDRenderer(LogSyslogFacility::User, "FreeBSDVTRenderer");

FreeBSDVTRenderer::FreeBSDVTRenderer(const char *device) : m_gui(nullptr)  {
    LogFreeBSDRenderer.print(LogPriority::Informational, "Opening %s", device);

    int fd = open(device, O_RDWR | O_CLOEXEC | O_NONBLOCK);
    if(fd < 0)
        throw FreeBSDError();

    m_vt.reset(fd);

    LogFreeBSDRenderer.print(LogPriority::Informational, "framebuffer open: %d", fd);

    struct fbtype fbType;
    unsigned int windowOrigin;
    video_display_start_t windowStart;
    unsigned int stride;

    if(ioctl(m_vt, FBIOGTYPE, &fbType) < 0 ||
       ioctl(m_vt, FBIO_GETWINORG, &windowOrigin) < 0 ||
       ioctl(m_vt, FBIO_GETDISPSTART, &windowStart) < 0 ||
       ioctl(m_vt, FBIO_GETLINEWIDTH, &stride) < 0)
        throw FreeBSDError();


    LogFreeBSDRenderer.print(LogPriority::Informational, "fb type: %d", fbType.fb_type);

    m_displayInfo.displayWidth = fbType.fb_width;
    m_displayInfo.displayHeight = fbType.fb_height;

    LogFreeBSDRenderer.print(LogPriority::Informational, "mapping");

    m_mapping.size = fbType.fb_size;
    void *base = mmap(nullptr, m_mapping.size, PROT_READ | PROT_WRITE, MAP_SHARED, m_vt, 0);
    if(base == MAP_FAILED)
        throw FreeBSDError();

    m_mapping.base = base;

    LogFreeBSDRenderer.print(
        LogPriority::Informational,
        "mapped. Base: %p, origin: %u. Width %d, height %d, stride %d",
        m_mapping.base, windowOrigin,
        fbType.fb_width, fbType.fb_height, stride
    );

    auto image = pixman_image_create_bits_no_clear(
        PIXMAN_a8b8g8r8,
        fbType.fb_width,
        fbType.fb_height,
        reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(m_mapping.base) + windowOrigin),
        stride
    );

    LogFreeBSDRenderer.print(LogPriority::Informational, "image: %p", image);

    assert(image);
    m_display.reset(image);

    LogFreeBSDRenderer.print(LogPriority::Informational, "ready");
}

FreeBSDVTRenderer::~FreeBSDVTRenderer() = default;

FreeBSDVTRenderer::Mapping::Mapping() : base(nullptr), size(0) {

}

FreeBSDVTRenderer::Mapping::~Mapping() {
    if(base)
        munmap(base, size);
}

void FreeBSDVTRenderer::drawFrame() {
    LogFreeBSDRenderer.print(LogPriority::Informational, "drawing frame\n");

    if(m_display && m_gui)
        m_gui->drawFrame(m_display.get());
}

void FreeBSDVTRenderer::deliverDeferred() {
    if (m_gui) {
        m_gui->dispatchDeferred();
    }
}
