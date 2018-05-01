#include <DSO/ApplicationInstance.h>
#include <DSO/DSOPlatform.h>
#include <DSO/DSOAcquisition.h>

#include <GUI/ApplicationPlatform.h>


ApplicationInstance::ApplicationInstance(std::unique_ptr<DSOPlatform> &&platform) :
    m_platform(std::move(platform)), m_app(m_platform->createGUIPlatform()), m_acquisition(m_platform->createAcquisition()) {

    m_app.setRootView(&m_rootView);
    m_rootView.initializeAcquisition(*m_acquisition);
}

ApplicationInstance::~ApplicationInstance() {

}

void ApplicationInstance::exec() {
    m_app.exec();
}
