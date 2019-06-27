#ifndef FREEBSD_PLATFORM__FREEBSD_MANAGED_DESCRIPTOR__H
#define FREEBSD_PLATFORM__FREEBSD_MANAGED_DESCRIPTOR__H

class FreeBSDManagedDescriptor {
public:
    explicit FreeBSDManagedDescriptor(int fd = -1) noexcept;
    ~FreeBSDManagedDescriptor();

    FreeBSDManagedDescriptor(const FreeBSDManagedDescriptor &other) = delete;
    FreeBSDManagedDescriptor &operator =(const FreeBSDManagedDescriptor &other) = delete;

    FreeBSDManagedDescriptor(FreeBSDManagedDescriptor &&other) noexcept;
    FreeBSDManagedDescriptor &operator=(FreeBSDManagedDescriptor &&other) noexcept;

    inline operator int() const noexcept {
        return m_fd;
    }

    void reset(int fd = -1);

    void swap(FreeBSDManagedDescriptor &other) noexcept;

    int release() noexcept;

private:
    int m_fd;
};

#endif
