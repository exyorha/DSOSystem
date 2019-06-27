#include <FreeBSDPlatform/FreeBSDApplicationPlatform.h>
#include <FreeBSDPlatform/FreeBSDDSOPlatform.h>

#include <Logging/LogFacility.h>

LogFacility LogFreeBSDApplicationPlatform(LogSyslogFacility::User, "FreeBSDApplicationPlatform");

FreeBSDApplicationPlatform::FreeBSDApplicationPlatform(FreeBSDDSOPlatform *platform) : m_platform(platform), m_gui(nullptr),
    m_renderer("/dev/ttyv0"), m_frameRequested(false) {

}

FreeBSDApplicationPlatform::~FreeBSDApplicationPlatform() {

}

DisplayInformation FreeBSDApplicationPlatform::displayInformation() const {
    return m_renderer.displayInfo();
}

void FreeBSDApplicationPlatform::requestFrame() {
    LogFreeBSDApplicationPlatform.print(LogPriority::Informational, "requestFrame(%d)...", m_frameRequested);

    if(!m_frameRequested) {
        m_frameRequested = true;
        m_platform->eventLoop().defer([this]() {
            if(m_frameRequested) {
                m_frameRequested = false;

                LogFreeBSDApplicationPlatform.print(LogPriority::Informational, "... drawFrame");

                m_renderer.drawFrame();
            }
        });
    }
}

void FreeBSDApplicationPlatform::keepPreviousFrame() {

}

void FreeBSDApplicationPlatform::setApplicationPlatformInterface(ApplicationPlatformInterface *iface) {
    m_gui = iface;
    m_renderer.setGui(iface);
}

void FreeBSDApplicationPlatform::exec() {
    m_platform->eventLoop().dispatch();
}

void FreeBSDApplicationPlatform::postDefer() {
    LogFreeBSDApplicationPlatform.print(LogPriority::Informational, "postDefer...");
    m_platform->eventLoop().defer([this]() {
        LogFreeBSDApplicationPlatform.print(LogPriority::Informational, "... deliverDeferred\n");

        m_renderer.deliverDeferred();
    });
}
