#include <LinuxPlatform/LinuxManagedDescriptor.h>
#include <LinuxPlatform/LinuxError.h>

#include <algorithm>

#include <unistd.h>

LinuxManagedDescriptor::LinuxManagedDescriptor(int fd) noexcept : m_fd(fd) {

}

LinuxManagedDescriptor::~LinuxManagedDescriptor() {
    if(m_fd >= 0) {
        close(m_fd);
	}
}

LinuxManagedDescriptor::LinuxManagedDescriptor(LinuxManagedDescriptor &&other) noexcept : m_fd(-1) {
    swap(other);
}
    
LinuxManagedDescriptor &LinuxManagedDescriptor::operator=(LinuxManagedDescriptor &&other) noexcept {
    swap(other);
    
    return *this;
}

void LinuxManagedDescriptor::reset(int fd) {
    LinuxManagedDescriptor newdesc(fd);
    
    swap(newdesc);
}

void LinuxManagedDescriptor::swap(LinuxManagedDescriptor &other) noexcept {
    std::swap(m_fd, other.m_fd);
}

int LinuxManagedDescriptor::release() noexcept {
    int fd = -1;

    std::swap(fd, m_fd);

    return fd;
}
