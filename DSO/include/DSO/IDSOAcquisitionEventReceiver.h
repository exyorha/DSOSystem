#ifndef DSO__I_DSO_ACQUISITION_EVENT_RECEIVER__H
#define DSO__I_DSO_ACQUISITION_EVENT_RECEIVER__H

class IDSOAcquisitionEventReceiver {
protected:
	inline IDSOAcquisitionEventReceiver() {}
	inline ~IDSOAcquisitionEventReceiver() {}

public:
	virtual void acquisitionStateChanged() = 0;
};

#endif
