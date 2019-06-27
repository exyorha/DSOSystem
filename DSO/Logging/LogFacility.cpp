#include <Logging/LogFacility.h>
#include <Logging/LogManager.h>

#include <stdio.h>

#include <vector>

LogFacility::LogFacility(LogSyslogFacility facility, const char *facilityString) : m_facility(facility), m_facilityString(facilityString) {

}

LogFacility::~LogFacility() {

}

int LogFacility::print(LogPriority priority, const char *format, ...) {
	va_list args;

	va_start(args, format);

	int result = vprint(priority, format, args);

	va_end(args);

	return result;
}

int LogFacility::vprint(LogPriority priority, const char *format, va_list args) {
#if defined(_WIN32)
	int length = _vscprintf(format, args);
	std::vector<char> buf(length + 1);

	int result = vsprintf_s(buf.data(), buf.size(), format, args);

	LogManager::instance()->message(priority, m_facility, m_facilityString, buf.data());

	return result;
#else
	struct WatchedBuffer {
		WatchedBuffer() : buf(nullptr) {}
		~WatchedBuffer() {
			free(buf);
		}

		char *buf;
	} watchedBuffer;

	int result = vasprintf(&watchedBuffer.buf, format, args);

	LogManager::instance()->message(priority, m_facility, m_facilityString, watchedBuffer.buf);

	return result;
#endif
}
