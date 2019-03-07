#ifndef DSO__DSO_ROOT_VIEW__H
#define DSO__DSO_ROOT_VIEW__H

#include <GUI/Window.h>
#include <DSO/IDSOAcquisitionEventReceiver.h>
#include <Logging/LogFacility.h>

class TextView;
class DSOScreenView;
class DSOAcquisition;

class DSORootView final : public Window, private IDSOAcquisitionEventReceiver {
public:
    DSORootView();
    virtual ~DSORootView();

    void initializeAcquisition(DSOAcquisition &acquisition);
	
private:
    virtual void acquisitionStateChanged() override;

private:
	static struct nk_rect getBounds();

    TextView *m_runState;
    DSOScreenView *m_screenView;
    TextView *m_bottomStatusBar;
    DSOAcquisition *m_acquisition;
};

extern LogFacility dsoRootViewLog;

#endif
