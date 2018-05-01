#ifndef LINUX_PLATFORM__LINUX_ERROR__H
#define LINUX_PLATFORM__LINUX_ERROR__H

#include <errno.h>
#include <exception>

class LinuxError final : public std::exception {
public:
    LinuxError(int error = errno) noexcept;
    virtual ~LinuxError();

    inline int error() const { return m_error; }
    
private:
    int m_error;
};

#endif