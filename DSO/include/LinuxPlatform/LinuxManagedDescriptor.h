#ifndef LINUX_PLATFORM__LINUX_MANAGED_DESCRIPTOR__H
#define LINUX_PLATFORM__LINUX_MANAGED_DESCRIPTOR__H

class LinuxManagedDescriptor {
public:
    explicit LinuxManagedDescriptor(int fd = -1) noexcept;
    ~LinuxManagedDescriptor();
    
    LinuxManagedDescriptor(const LinuxManagedDescriptor &other) = delete;
    LinuxManagedDescriptor &operator =(const LinuxManagedDescriptor &other) = delete;
    
    LinuxManagedDescriptor(LinuxManagedDescriptor &&other) noexcept;
    LinuxManagedDescriptor &operator=(LinuxManagedDescriptor &&other) noexcept;

    inline operator int() const noexcept {
        return m_fd;
    }
    
    void reset(int fd = -1);
    
    void swap(LinuxManagedDescriptor &other) noexcept;
    
    int release() noexcept;

private:
    int m_fd;
};
        
#endif