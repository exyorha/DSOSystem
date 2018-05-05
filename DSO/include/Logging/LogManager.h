#ifndef LOGGING_LOG_MANAGER__H
#define LOGGING_LOG_MANAGER__H

#include <shared_mutex>
#include <unordered_set>
#include <Logging/ILogSink.h>

class LogManager {
protected:
	LogManager();
	~LogManager();

public:
	LogManager(const LogManager &other) = delete;
	LogManager &operator =(const LogManager &other) = delete;

	void registerSink(ILogSink *sink);
	void unregisterSink(ILogSink *sink);

	void message(LogPriority priority, LogSyslogFacility facility, const char *facilityString, const char *message);

	static LogManager *instance();

private:
	std::shared_mutex m_sinkListMutex;
	std::unordered_set<ILogSink *> m_sinkList;
	static LogManager *m_instance;
};

const char *priorityString(LogPriority priority);
const char *facilityString(LogSyslogFacility facility);

#endif
