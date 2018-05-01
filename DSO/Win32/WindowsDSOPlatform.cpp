#include <Win32/WindowsDSOPlatform.h>
#include <Win32/WindowsApplicationPlatform.h>
#include <Win32/WindowsAcquisition.h>

WindowsDSOPlatform::WindowsDSOPlatform() {

}

WindowsDSOPlatform::~WindowsDSOPlatform() {

}

std::unique_ptr<ApplicationPlatform> WindowsDSOPlatform::createGUIPlatform() {
    return std::make_unique<WindowsApplicationPlatform>();
}

std::unique_ptr<DSOAcquisition> WindowsDSOPlatform::createAcquisition() {
	return std::make_unique<WindowsAcquisition>();
}
