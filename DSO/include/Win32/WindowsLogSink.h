#ifndef WIN32__WINDOWS_LOG_SINK__H
#define WIN32__WINDOWS_LOG_SINK__H

#include <Logging/LogSink.h>

class WindowsLogSink final : public LogSink {
public:
	WindowsLogSink();
	~WindowsLogSink();

	virtual void message(LogPriority priority, LogSyslogFacility facility, const char *facilityString, const char *message) override;

private:
	bool m_isConsole;
};

#endif
