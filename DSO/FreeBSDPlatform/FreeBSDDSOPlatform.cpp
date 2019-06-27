#include <FreeBSDPlatform/FreeBSDDSOPlatform.h>
#include <FreeBSDPlatform/FreeBSDApplicationPlatform.h>
#include <FreeBSDPlatform/FreeBSDAcquisition.h>

FreeBSDDSOPlatform::FreeBSDDSOPlatform() {

}

FreeBSDDSOPlatform::~FreeBSDDSOPlatform() = default;

std::unique_ptr<ApplicationPlatform> FreeBSDDSOPlatform::createGUIPlatform() {
    return std::unique_ptr<ApplicationPlatform>(new FreeBSDApplicationPlatform(this));
}

std::unique_ptr<DSOAcquisition> FreeBSDDSOPlatform::createAcquisition() {
    return std::unique_ptr<DSOAcquisition>(new FreeBSDAcquisition());
}
