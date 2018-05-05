#ifndef DSO__DSO_ROOT_VIEW__H
#define DSO__DSO_ROOT_VIEW__H

#include <GUI/View.h>
#include <DSO/IDSOAcquisitionEventReceiver.h>
#include <Logging/LogFacility.h>

class TextView;
class DSOScreenView;
class DSOAcquisition;

class DSORootView final : public View, private IDSOAcquisitionEventReceiver {
public:
    DSORootView();
    virtual ~DSORootView();

    void initializeAcquisition(DSOAcquisition &acquisition);

protected:
	virtual void keyPressEvent(KeyEvent *event) override;
	virtual void keyReleaseEvent(KeyEvent *event) override;

private:
    virtual void acquisitionStateChanged() override;

private:
    TextView *m_runState;
    DSOScreenView *m_screenView;
    TextView *m_bottomStatusBar;
    DSOAcquisition *m_acquisition;
};

extern LogFacility dsoRootViewLog;

#endif
