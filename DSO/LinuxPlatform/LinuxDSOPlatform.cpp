#include <LinuxPlatform/LinuxDSOPlatform.h>
#include <LinuxPlatform/LinuxApplicationPlatform.h>
#include <LinuxPlatform/LinuxAcquisition.h>

LinuxDSOPlatform::LinuxDSOPlatform() {

}

LinuxDSOPlatform::~LinuxDSOPlatform() {

}
    
std::unique_ptr<ApplicationPlatform> LinuxDSOPlatform::createGUIPlatform() {
    return std::unique_ptr<ApplicationPlatform>(new LinuxApplicationPlatform(this));
}
    
std::unique_ptr<DSOAcquisition> LinuxDSOPlatform::createAcquisition() {
    return std::unique_ptr<DSOAcquisition>(new LinuxAcquisition());
}