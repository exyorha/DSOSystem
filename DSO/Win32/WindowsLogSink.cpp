#include <Win32/WindowsLogSink.h>
#include <Logging/LogManager.h>

#include <sstream>

#include <Windows.h>

WindowsLogSink::WindowsLogSink() {
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	m_isConsole = GetConsoleMode(stdoutHandle, &mode) != 0;

	if (!m_isConsole) {
		wchar_t bom = 0xFEFF;
		DWORD bytesWritten;
		WriteFile(stdoutHandle, &bom, sizeof(bom), &bytesWritten, nullptr);
	}
}

WindowsLogSink::~WindowsLogSink() {

}

void WindowsLogSink::message(LogPriority priority, LogSyslogFacility facility, const char *facilityString, const char *message) {
	std::stringstream formattedMessage;
	formattedMessage << priorityString(priority) << "/" << ::facilityString(facility) << " " << facilityString << ": " << message << "\n";
	
	auto string = formattedMessage.str();

	std::wstring output;
	output.resize(string.size() * 2); // Worst case: every UTF-8 byte generates two UTF-16 wchars_t

	int characters = MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), &output[0], (int)output.size());
	if (characters == 0)
		throw std::runtime_error("UTF-8 conversion failed");

	output.resize(characters);
	
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD written;

	if (m_isConsole) {
		WriteConsole(stdoutHandle, output.data(), (DWORD)output.size(), &written, nullptr);
	}
	else {
		WriteFile(stdoutHandle, output.data(), (DWORD)output.size() * sizeof(wchar_t), &written, nullptr);
	}
}
