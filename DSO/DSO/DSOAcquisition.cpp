#include <DSO/DSOAcquisition.h>
#include <DSO/IDSOAcquisitionEventReceiver.h>

#include <GUI/Application.h>

DSOAcquisition::DSOAcquisition() : m_state(State::Stopped), m_triggerMode(TriggerMode::Auto) {

}

DSOAcquisition::~DSOAcquisition() {

}

auto DSOAcquisition::state() const -> State {
	std::unique_lock<std::mutex> lock(m_acquisitionMutex);

	return m_state;
}

void DSOAcquisition::start() {
	std::unique_lock<std::mutex> lock(m_acquisitionMutex);

	if (m_state == State::Stopped) {
		internalSetState(State::Starting);
		internalStart();
	}
}

void DSOAcquisition::stop() {
	std::unique_lock<std::mutex> lock(m_acquisitionMutex);

	if (m_state == State::Running) {
		internalSetState(State::Stopping);
		internalStop();
	}
}

void DSOAcquisition::started() {
	std::unique_lock<std::mutex> lock(m_acquisitionMutex);

	internalSetState(State::Running);
}

void DSOAcquisition::stopped() {
	std::unique_lock<std::mutex> lock(m_acquisitionMutex);

	internalSetState(State::Stopped);
}

void DSOAcquisition::internalSetState(State state) {
	m_state = state;

	Application::instance()->defer(std::bind(&DSOAcquisition::deliverStateChange, this));
}

void DSOAcquisition::deliverStateChange() {
	for (auto receiver : m_eventReceivers) {
		receiver->acquisitionStateChanged();
	}
}

auto DSOAcquisition::triggerMode() const -> TriggerMode {
	return m_triggerMode;
}

void DSOAcquisition::setTriggerMode(TriggerMode mode) {
	if (mode != m_triggerMode) {
		m_triggerMode = mode;
		internalSetTriggerMode(mode);
	}
}

void DSOAcquisition::registerEventReceiver(IDSOAcquisitionEventReceiver *receiver) {
	m_eventReceivers.emplace(receiver);
}

void DSOAcquisition::unregisterEventReceiver(IDSOAcquisitionEventReceiver *receiver) {
	m_eventReceivers.erase(receiver);
}
