#include <LinuxPlatform/LinuxError.h>

#include <sstream>

#include <string.h>

LinuxError::LinuxError(int error) noexcept : m_error(error) {

}

LinuxError::~LinuxError() {

}
