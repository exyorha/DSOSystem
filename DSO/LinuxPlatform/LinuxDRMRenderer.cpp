#include <LinuxPlatform/LinuxDRMRenderer.h>
#include <LinuxPlatform/LinuxError.h>
#include <LinuxPlatform/LinuxEventLoop.h>
#include <GUI/ApplicationPlatformInterface.h>

#include <fcntl.h>
#include <xf86drmMode.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <SkSurface.h>

#include <memory>

struct DRMModeResourcesDeleter {
    void operator()(drmModeRes *res) {
        drmModeFreeResources(res);
    }
};

struct DRMModeConnectorDeleter {
    void operator()(drmModeConnector *res) {
        drmModeFreeConnector(res);
    }
};

struct DRMModeEncoderDeleter {
    void operator()(drmModeEncoder *res) {
        drmModeFreeEncoder(res);
    }
};

struct DRMModeCRTCDeleter {
    void operator()(drmModeCrtc *res) {
        drmModeFreeCrtc(res);
    }
};

LinuxDRMRenderer::LinuxDRMRenderer(const char *device, LinuxEventLoop &eventLoop) : m_eventLoop(eventLoop),
    m_readPointer(0), m_writePointer(1), m_buffersUsed(1), m_swapPending(true), m_gui(nullptr)  {

    int fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC | O_NONBLOCK);
    if(fd < 0)
        throw LinuxError();

    m_drm.reset(fd);

    uint64_t hasDumb;
    if(drmGetCap(m_drm, DRM_CAP_DUMB_BUFFER, &hasDumb) < 0 || !hasDumb)
        throw std::runtime_error("dumb buffers are not supported");

    std::unique_ptr<drmModeRes, DRMModeResourcesDeleter> modeRes(drmModeGetResources(m_drm));
    if(!modeRes)
        throw LinuxError();

    for(auto connector = modeRes->connectors;
        connector < modeRes->connectors + modeRes->count_connectors;
        connector++) {

        std::unique_ptr<drmModeConnector, DRMModeConnectorDeleter> info(drmModeGetConnector(m_drm, *connector));

        printf("connection: %d\n", info->connection);

        for(auto mode = info->modes; mode < info->modes + info->count_modes; mode++) {
            printf("Mode: %ux%u\n", mode->hdisplay, mode->vdisplay);

            m_displayInfo.displaySize = SkISize::Make(mode->hdisplay, mode->vdisplay);

            m_screenBuffers.reserve(3);

            for(unsigned int index = 0; index < 3; index++) {
                ScreenBuffer buffer;
                buffer.width = m_displayInfo.displaySize.width();
                buffer.height = m_displayInfo.displaySize.height();

                drm_mode_create_dumb bufreq;
                memset(&bufreq, 0, sizeof(bufreq));
                bufreq.height = buffer.height;
                bufreq.width = buffer.width;
                bufreq.bpp = 32;
                
                int result = ioctl(m_drm, DRM_IOCTL_MODE_CREATE_DUMB, &bufreq);
                if(result < 0)
                    throw LinuxError();

                buffer.handle = bufreq.handle;
                buffer.pitch = bufreq.pitch;
                buffer.size = bufreq.size;

                result = drmModeAddFB(m_drm,
                    buffer.width,
                    buffer.height,
                    24,
                    32,
                    buffer.pitch,
                    buffer.handle,
                    &buffer.fbObject);
                if(result < 0)
                    throw LinuxError();
                    
                drm_mode_map_dumb mapreq;
                memset(&mapreq, 0, sizeof(mapreq));
                mapreq.handle = buffer.handle;

                result = ioctl(m_drm, DRM_IOCTL_MODE_MAP_DUMB, &mapreq);
                if(result < 0)
                    throw LinuxError();

                buffer.mapping = mmap(nullptr, buffer.size, PROT_READ | PROT_WRITE, MAP_SHARED, m_drm, mapreq.offset);
                if(buffer.mapping == MAP_FAILED)
                    throw LinuxError();

                memset(buffer.mapping, 0, buffer.size);

                buffer.surface = SkSurface::MakeRasterDirect(
                    SkImageInfo::MakeN32(buffer.width, buffer.height, kOpaque_SkAlphaType), 
                    buffer.mapping,
                    buffer.pitch);
                
                m_screenBuffers.emplace_back(std::move(buffer));
            }

            if(info->encoder_id == 0) {
                for(uint32_t *encoder = info->encoders; encoder < info->encoders + info->count_encoders; encoder++) {
                    std::unique_ptr<drmModeEncoder, DRMModeEncoderDeleter> encoderInfo(drmModeGetEncoder(m_drm, *encoder));
                    if(!encoderInfo)
                        throw LinuxError();

                    info->encoder_id = *encoder;
                    break;
                }
            }

            if(info->encoder_id == 0)
                throw std::runtime_error("No suitable encoder");

            std::unique_ptr<drmModeEncoder, DRMModeEncoderDeleter> encoder(drmModeGetEncoder(m_drm, info->encoder_id));
            if(!encoder)
                throw LinuxError();

            if(encoder->crtc_id == 0) {
                for(unsigned int crtcIndex = 0; crtcIndex < modeRes->count_crtcs; crtcIndex++) {
                    if(encoder->possible_crtcs & (1 << crtcIndex)) {
                        encoder->crtc_id = modeRes->crtcs[crtcIndex];
                        break;
                    }
                }
            }

            printf("Setting mode: CRTC %d, connector %d, buffer %d\n",
                encoder->crtc_id,
                *connector,
                m_screenBuffers.front().fbObject);
                
            int result = drmModeSetCrtc(m_drm,
                encoder->crtc_id,
                m_screenBuffers.front().fbObject,
                0, 0,
                connector, 1,
                mode);
            if(result < 0)
                throw LinuxError();

            m_crtc = encoder->crtc_id;

            goto breakOuter;
        }
    }
breakOuter:
    
    if(m_screenBuffers.empty())
        throw std::runtime_error("no suitable connector");

    int result = drmModePageFlip(m_drm, m_crtc, m_screenBuffers[1].fbObject, DRM_MODE_PAGE_FLIP_EVENT, this);
    if(result < 0) {
        perror("drmModePageFlip");
    }
    if(result < 0)
        throw LinuxError();

    m_eventLoop.registerDescriptor(m_drm, this, EPOLLIN);
}

LinuxDRMRenderer::~LinuxDRMRenderer() {
    m_eventLoop.unregisterDescriptor(m_drm);
}

LinuxDRMRenderer::ScreenBuffer::ScreenBuffer() : mapping(nullptr), size(0) {

}

LinuxDRMRenderer::ScreenBuffer::~ScreenBuffer() {
    if(mapping) {
        munmap(mapping, size);
    }
}

LinuxDRMRenderer::ScreenBuffer::ScreenBuffer(ScreenBuffer &&other)  : mapping(nullptr), size(0) {
    width = other.width;
    height = other.height;
    handle = other.handle;
    pitch = other.pitch;
    std::swap(size, other.size);
    std::swap(mapping, other.mapping);
    fbObject = other.fbObject;
    surface = other.surface;
}

auto LinuxDRMRenderer::ScreenBuffer::operator =(ScreenBuffer &&other) -> ScreenBuffer & {
    width = other.width;
    height = other.height;
    handle = other.handle;
    pitch = other.pitch;
    std::swap(size, other.size);
    std::swap(mapping, other.mapping);
    fbObject = other.fbObject;
    surface = other.surface;

    return *this;
}

void LinuxDRMRenderer::handleEvents(unsigned int events) {
    if(events & EPOLLIN) {
        if(drmHandleEvent(m_drm, const_cast<drmEventContextPtr>(&m_drmEventContext)) < 0)
            throw LinuxError();
    }
}

void LinuxDRMRenderer::pageFlipHandlerThunk(
    int fd, unsigned int sequence, unsigned int tv_sec, unsigned int tv_usec, unsigned int crtc_id, void *user_data) {

    static_cast<LinuxDRMRenderer *>(user_data)->pageFlipHandler();
}

void LinuxDRMRenderer::keepPreviousFrame() {
    size_t prevPointer;
    if(m_writePointer == 0)
        prevPointer = m_screenBuffers.size() - 1;
    else
        prevPointer = m_writePointer - 1;

    auto &currentBuffer = m_screenBuffers[m_writePointer];
    auto &previousBuffer = m_screenBuffers[prevPointer];

    for(unsigned int line = 0; line < currentBuffer.height; line++) {
        memcpy(
            static_cast<unsigned char *>(currentBuffer.mapping) + currentBuffer.pitch * line,
            static_cast<unsigned char *>(previousBuffer.mapping) + previousBuffer.pitch * line,
            sizeof(uint32_t) * currentBuffer.width);
    }
}

void LinuxDRMRenderer::pageFlipHandler() {
    if(++m_readPointer == m_screenBuffers.size())
        m_readPointer = 0;
    m_buffersUsed--;
    m_swapPending = false;

    if(m_buffersUsed > 0) {
        int result = drmModePageFlip(m_drm, m_crtc, m_screenBuffers[m_readPointer].fbObject, DRM_MODE_PAGE_FLIP_EVENT, this);
        if(result < 0)
            throw LinuxError();

        m_swapPending = true;
    }

    while(m_buffersUsed < m_screenBuffers.size()) {
        auto &buffer = m_screenBuffers[m_writePointer];

        m_gui->drawFrame(buffer.surface);

        if(++m_writePointer == m_screenBuffers.size())
            m_writePointer = 0;
        m_buffersUsed++;
    }

    if(!m_swapPending) {
        int result = drmModePageFlip(m_drm, m_crtc, m_screenBuffers[m_readPointer].fbObject, DRM_MODE_PAGE_FLIP_EVENT, this);
        if(result < 0)
            throw LinuxError();

        m_swapPending = true; 
    }
}
    
const drmEventContext LinuxDRMRenderer::m_drmEventContext = {
    .version = DRM_EVENT_CONTEXT_VERSION,
    .vblank_handler = nullptr,
    .page_flip_handler = nullptr,
    .page_flip_handler2 = &LinuxDRMRenderer::pageFlipHandlerThunk,
    .sequence_handler = nullptr
};
