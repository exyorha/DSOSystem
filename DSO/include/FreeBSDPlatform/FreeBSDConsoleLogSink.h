#ifndef FREEBSD_PLATFORM_FREEBSD_CONSOLE_LOG_SINK_H
#define FREEBSD_PLATFORM_FREEBSD_CONSOLE_LOG_SINK_H

#include <Logging/LogSink.h>
#include <FreeBSDPlatform/FreeBSDManagedDescriptor.h>

class FreeBSDConsoleLogSink final : public LogSink {
public:
    FreeBSDConsoleLogSink();
    ~FreeBSDConsoleLogSink();

	virtual void message(LogPriority priority, LogSyslogFacility facility, const char *facilityString, const char *message) override;

private:
    FreeBSDManagedDescriptor m_console;
};

#endif
