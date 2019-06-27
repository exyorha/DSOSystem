#ifndef FREEBSD_PLATFORM__FREEBSD_ERROR__H
#define FREEBSD_PLATFORM__FREEBSD_ERROR__H

#include <errno.h>
#include <exception>

class FreeBSDError final : public std::exception {
public:
    FreeBSDError(int error = errno) noexcept;
    virtual ~FreeBSDError();

    inline int error() const { return m_error; }

private:
    int m_error;
};

#endif
