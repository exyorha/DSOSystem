#ifndef DSO__APPLICATION_INSTANCE__H
#define DSO__APPLICATION_INSTANCE__H

#include <GUI/Application.h>
#include <DSO/DSORootView.h>

class DSOPlatform;
class DSOAcquisition;

class ApplicationInstance final {
public:
    ApplicationInstance(std::unique_ptr<DSOPlatform> &&platform);
    ~ApplicationInstance();

    ApplicationInstance(const ApplicationInstance &other) = delete;
    ApplicationInstance &operator =(const ApplicationInstance &other) = delete;

    void exec();

private:
    std::unique_ptr<DSOPlatform> m_platform;
    std::unique_ptr<DSOAcquisition> m_acquisition;
    Application m_app;
    DSORootView m_rootView;
};

#endif
