#ifndef LOGGING_LOG_SINK__H
#define LOGGING_LOG_SINK__H

#include <Logging/ILogSink.h>

class LogSink : public ILogSink {
protected:
	LogSink();
	~LogSink();
};

#endif
