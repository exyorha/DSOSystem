#include <Win32/WindowsAcquisition.h>
#include <Win32/WindowsDSOAccumulator.h>

WindowsAcquisition::WindowsAcquisition() : m_runEngine(true), m_running(false), m_runRequest(false), m_engineThread(&WindowsAcquisition::engineThread, this) {

}

WindowsAcquisition::~WindowsAcquisition() {
	{
		std::unique_lock<std::mutex> locker(m_engineMutex);
		m_runEngine = false;
	}

	m_engineCondvar.notify_all();
		
	m_engineThread.join();
}

size_t WindowsAcquisition::channelCount() const {
	return 1;
}

std::unique_ptr<DSOAccumulator> WindowsAcquisition::allocateChannelAccumulator(size_t channel, size_t width, size_t height) {
	return std::make_unique<WindowsDSOAccumulator>(width, height);
}

void WindowsAcquisition::engineThread() {
	while (true) {
		std::unique_lock<std::mutex> locker(m_engineMutex);

		while (m_runEngine && !m_runRequest) {
			m_engineCondvar.wait(locker);
		}

		if (!m_runEngine)
			return;

		m_running = m_runRequest;

		if (m_running)
			started();

		while (m_running) {

			while (m_runEngine && m_runRequest) {
				m_engineCondvar.wait(locker);
			}

			if (!m_runEngine)
				return;

			m_running = m_runRequest;

			if (!m_running) {
				stopped();
				break;
			}
		}
	}
}

void WindowsAcquisition::internalStart() {
	std::unique_lock<std::mutex> locker(m_engineMutex);
	m_runRequest = true;
	m_engineCondvar.notify_one();
}

void WindowsAcquisition::internalStop() {
	std::unique_lock<std::mutex> locker(m_engineMutex);
	m_runRequest = false;
	m_engineCondvar.notify_one();
}

void WindowsAcquisition::internalSetTriggerMode(TriggerMode mode) {

}

