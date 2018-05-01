#ifndef WIN32__WINDOWS_ACQUSITION__H
#define WIN32__WINDOWS_ACQUSITION__H

#include <DSO/DSOAcquisition.h>
#include <thread>
#include <mutex>

class WindowsAcquisition final : public DSOAcquisition {
public:
	WindowsAcquisition();
	virtual ~WindowsAcquisition();

	virtual size_t channelCount() const override;
	virtual std::unique_ptr<DSOAccumulator> allocateChannelAccumulator(size_t channel, size_t width, size_t height) override;

protected:
	virtual void internalStart() override;
	virtual void internalStop() override;
	virtual void internalSetTriggerMode(TriggerMode mode) override;

private:
	void engineThread(void);

	std::mutex m_engineMutex;
	std::condition_variable m_engineCondvar;
	bool m_runEngine;
	bool m_running, m_runRequest;
	std::thread m_engineThread;
};

#endif
