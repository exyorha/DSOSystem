#ifndef DSO__DSO_PLATFORM__H
#define DSO__DSO_PLATFORM__H

#include <memory>

class ApplicationPlatform;
class DSOAcquisition;

class DSOPlatform {
protected:
    DSOPlatform();

public:
    virtual ~DSOPlatform();

    DSOPlatform(const DSOPlatform &other) = delete;
    DSOPlatform &operator =(const DSOPlatform &other) = delete;

    virtual std::unique_ptr<ApplicationPlatform> createGUIPlatform() = 0;
    virtual std::unique_ptr<DSOAcquisition> createAcquisition() = 0;
};

#endif
