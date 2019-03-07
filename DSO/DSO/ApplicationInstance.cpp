#include <DSO/ApplicationInstance.h>
#include <DSO/DSOPlatform.h>
#include <DSO/DSOAcquisition.h>
#include <DSO/DSORootView.h>

#include <GUI/ApplicationPlatform.h>


ApplicationInstance::ApplicationInstance(std::unique_ptr<DSOPlatform> &&platform) :
    m_platform(std::move(platform)), m_app(m_platform->createGUIPlatform()), m_acquisition(m_platform->createAcquisition()) {

	auto view = std::unique_ptr<DSORootView>(new DSORootView());
	auto viewPtr = view.get();
	m_app.addWindow(std::move(view));
    viewPtr->initializeAcquisition(*m_acquisition);
}

ApplicationInstance::~ApplicationInstance() {

}

void ApplicationInstance::exec() {
    m_app.exec();
}
