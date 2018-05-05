#ifndef LOGGING_I_LOG_SINK__H
#define LOGGING_I_LOG_SINK__H

enum class LogPriority {
	Emergency,
	Alert,
	Critical,
	Error,
	Warning,
	Notice,
	Informational,
	Debug
};

enum class LogSyslogFacility {
	Kernel,
	User
};

class ILogSink {
protected:
	inline ILogSink() { }
	inline ~ILogSink() { }

public:
	ILogSink(const ILogSink &other) = delete;
	ILogSink &operator =(const ILogSink &other) = delete;

	virtual void message(LogPriority priority, LogSyslogFacility facility, const char *facilityString, const char *message) = 0;
};

#endif
