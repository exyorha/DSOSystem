#include <Logging/LogManager.h>

LogManager::LogManager() {

}

LogManager::~LogManager() {

}

void LogManager::registerSink(ILogSink *sink) {
	std::unique_lock<std::shared_mutex> locker(m_sinkListMutex);
	m_sinkList.emplace(sink);
}

void LogManager::unregisterSink(ILogSink *sink) {
	std::unique_lock<std::shared_mutex> locker(m_sinkListMutex);
	m_sinkList.erase(sink);
}

void LogManager::message(LogPriority priority, LogSyslogFacility facility, const char *facilityString, const char *message) {
	std::shared_lock<std::shared_mutex> locker(m_sinkListMutex);

	for (auto sink : m_sinkList) {
		sink->message(priority, facility, facilityString, message);
	}
}

LogManager *LogManager::instance() {
	if (!m_instance)
		m_instance = new LogManager();
	return m_instance;
}

LogManager *LogManager::m_instance;

const char *priorityString(LogPriority priority) {
	const char *priorityStrings[] = {
			"emerg", "alert", "crit", "err", "warning", "notice", "info", "debug"
	};

	return priorityStrings[static_cast<size_t>(priority)];
}

const char *facilityString(LogSyslogFacility facility) {
	const char *facilityStrings[] = {
		"kern", "user"
	};

	return facilityStrings[static_cast<size_t>(facility)];
}
