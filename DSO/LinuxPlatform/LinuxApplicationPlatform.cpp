#include <LinuxPlatform/LinuxApplicationPlatform.h>
#include <LinuxPlatform/LinuxDSOPlatform.h>

LinuxApplicationPlatform::LinuxApplicationPlatform(LinuxDSOPlatform *platform) : m_platform(platform), m_gui(nullptr),
    m_renderer("/dev/dri/card0", m_platform->eventLoop()) {

}

LinuxApplicationPlatform::~LinuxApplicationPlatform() {

}

DisplayInformation LinuxApplicationPlatform::displayInformation() const {
    return  m_renderer.displayInfo();
}

void LinuxApplicationPlatform::requestFrame() {
    printf("Request frame\n");
}

void LinuxApplicationPlatform::keepPreviousFrame() {
    m_renderer.keepPreviousFrame();
}

void LinuxApplicationPlatform::setApplicationPlatformInterface(ApplicationPlatformInterface *iface) {
    m_gui = iface;
    m_renderer.setGui(iface);
}

void LinuxApplicationPlatform::exec() {
    m_platform->eventLoop().dispatch();
}
