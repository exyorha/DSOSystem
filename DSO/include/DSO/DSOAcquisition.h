#ifndef DSO__DSO_ACQUISITION__H
#define DSO__DSO_ACQUISITION__H

#include <memory>
#include <mutex>
#include <unordered_set>

class DSOAccumulator;
class IDSOAcquisitionEventReceiver;

class DSOAcquisition {
public:
	enum class State {
		Stopped,
		Starting,
		Running,
		Stopping
	};

	enum class TriggerMode {
		Auto,
		Normal,
		Single
	};

	DSOAcquisition();
	virtual ~DSOAcquisition();

	DSOAcquisition(const DSOAcquisition &other) = delete;
	DSOAcquisition &operator =(const DSOAcquisition &other) = delete;

	State state() const;
	void start();
	void stop();

	TriggerMode triggerMode() const;
	void setTriggerMode(TriggerMode mode);

	virtual size_t channelCount() const = 0;
	virtual std::unique_ptr<DSOAccumulator> allocateChannelAccumulator(size_t channel, size_t width, size_t height) = 0;

	void registerEventReceiver(IDSOAcquisitionEventReceiver *receiver);
	void unregisterEventReceiver(IDSOAcquisitionEventReceiver *receiver);

protected:
	virtual void internalStart() = 0;
	virtual void internalStop() = 0;
	virtual void internalSetTriggerMode(TriggerMode mode) = 0;

	void started();
	void stopped();

private:
	void internalSetState(State state);

	mutable std::mutex m_acquisitionMutex;
	State m_state;
	TriggerMode m_triggerMode;
	std::unordered_set<IDSOAcquisitionEventReceiver *> m_eventReceivers;
};

#endif
