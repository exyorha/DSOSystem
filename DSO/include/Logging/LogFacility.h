#ifndef LOGGING__LOG_FACILITY__H
#define LOGGING__LOG_FACILITY__H

#include <Logging/ILogSink.h>

#include <stdarg.h>

class LogFacility {
public:
	LogFacility(LogSyslogFacility facility, const char *facilityString);
	~LogFacility();

	LogFacility(const LogFacility &other) = delete;
	LogFacility &operator =(const LogFacility &other) = delete;

	int print(LogPriority priority, const char *format, ...);
	int vprint(LogPriority priority, const char *format, va_list args);

private:
	LogSyslogFacility m_facility;
	const char *m_facilityString;
};

#endif

