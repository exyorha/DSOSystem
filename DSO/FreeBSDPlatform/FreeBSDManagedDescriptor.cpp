#include <FreeBSDPlatform/FreeBSDManagedDescriptor.h>
#include <FreeBSDPlatform/FreeBSDError.h>

#include <algorithm>

#include <unistd.h>

FreeBSDManagedDescriptor::FreeBSDManagedDescriptor(int fd) noexcept : m_fd(fd) {

}

FreeBSDManagedDescriptor::~FreeBSDManagedDescriptor() {
    if(m_fd >= 0) {
        close(m_fd);
	}
}

FreeBSDManagedDescriptor::FreeBSDManagedDescriptor(FreeBSDManagedDescriptor &&other) noexcept : m_fd(-1) {
    swap(other);
}

FreeBSDManagedDescriptor &FreeBSDManagedDescriptor::operator=(FreeBSDManagedDescriptor &&other) noexcept {
    swap(other);

    return *this;
}

void FreeBSDManagedDescriptor::reset(int fd) {
    FreeBSDManagedDescriptor newdesc(fd);

    swap(newdesc);
}

void FreeBSDManagedDescriptor::swap(FreeBSDManagedDescriptor &other) noexcept {
    std::swap(m_fd, other.m_fd);
}

int FreeBSDManagedDescriptor::release() noexcept {
    int fd = -1;

    std::swap(fd, m_fd);

    return fd;
}
