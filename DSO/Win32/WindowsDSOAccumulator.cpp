#include <Win32/WindowsDSOAccumulator.h>
#include <Windows.h>
#include <atlbase.h>

WindowsDSOAccumulator::WindowsDSOAccumulator(size_t width, size_t height) : m_base(nullptr), m_pitch(width) {
	m_base = VirtualAlloc(nullptr, m_pitch * height, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!m_base)
		AtlThrow(HRESULT_FROM_WIN32(GetLastError()));
}

WindowsDSOAccumulator::~WindowsDSOAccumulator() {
	VirtualFree(m_base, 0, MEM_RELEASE);
}

const void *WindowsDSOAccumulator::base() const {
	return m_base;
}

size_t WindowsDSOAccumulator::pitch() const {
	return m_pitch;
}

