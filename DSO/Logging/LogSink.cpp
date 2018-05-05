#include <Logging/LogSink.h>
#include <Logging/LogManager.h>

LogSink::LogSink() {
	LogManager::instance()->registerSink(this);
}

LogSink::~LogSink() {
	LogManager::instance()->unregisterSink(this);
}
