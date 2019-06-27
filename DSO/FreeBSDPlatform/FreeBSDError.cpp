#include <FreeBSDPlatform/FreeBSDError.h>

#include <sstream>

#include <string.h>

FreeBSDError::FreeBSDError(int error) noexcept : m_error(error) {

}

FreeBSDError::~FreeBSDError() = default;
