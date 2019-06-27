#include <FreeBSDPlatform/FreeBSDConsoleLogSink.h>
#include <FreeBSDPlatform/FreeBSDError.h>

#include <Logging/LogManager.h>

#include <fcntl.h>
#include <unistd.h>

#include <sstream>

FreeBSDConsoleLogSink::FreeBSDConsoleLogSink() {
    auto fd = open("/dev/console", O_RDWR | O_CLOEXEC | O_NOCTTY);
    if(fd < 0) {
        throw FreeBSDError();
    }
    m_console.reset(fd);
}

FreeBSDConsoleLogSink::~FreeBSDConsoleLogSink() = default;

void FreeBSDConsoleLogSink::message(LogPriority priority, LogSyslogFacility facility, const char *facilityString, const char *message) {
    std::stringstream formattedMessage;
	formattedMessage << priorityString(priority) << "/" << ::facilityString(facility) << " " << facilityString << ": " << message << "\n";

	auto string = formattedMessage.str();

    auto ptr = string.data();
    auto size = string.size();
    while(size > 0) {
        auto bytesWritten = write(m_console, ptr, size);
        if(bytesWritten < 0) {
            if(errno == EINTR) {
                continue;
            }

            throw FreeBSDError();
        }

        if(bytesWritten == 0)
            break;

        ptr += bytesWritten;
        size -= bytesWritten;
    }
}
