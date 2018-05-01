#ifndef LINUX_PLATFORM__LINUX_ACQUISITION__H
#define LINUX_PLATFORM__LINUX_ACQUISITION__H

#include <DSO/DSOAcquisition.h>

class LinuxAcquisition final : public DSOAcquisition {
public:
    LinuxAcquisition();
    ~LinuxAcquisition();

  	virtual size_t channelCount() const override;
	virtual std::unique_ptr<DSOAccumulator> allocateChannelAccumulator(size_t channel, size_t width, size_t height) override;

protected:
	virtual void internalStart() override;
	virtual void internalStop() override;
	virtual void internalSetTriggerMode(TriggerMode mode) override;  
};

#endif