#ifndef INIT_CONFIGURATION__H
#define INIT_CONFIGURATION__H

#include "FieldBasedFileParser.h"

#include <string>

class InitConfiguration final : public FieldBasedFileParser {
public:
    struct Mount {
        std::string device;
        std::string filesystemType;
        std::string target;
        unsigned int mountOptions;
        bool mkdir;

        void execute() const;
    };

    struct Unmount {
        std::string target;
        unsigned int flags;

        void execute() const;
    };

    struct Run {
        std::string program;
        std::vector<std::string> arguments;

        pid_t execute() const;
    };

    struct Sysctl {
        std::string key;
        std::string value;

        void execute() const;
    };

    InitConfiguration();
    ~InitConfiguration();

    inline const std::vector<Mount> &mounts() const { return m_mounts; }
    inline const std::vector<Unmount> &unmounts() const { return m_unmounts; }
    inline const std::vector<Run> &runs() const { return m_runs; }
    inline const std::vector<Sysctl> &sysctls() const { return m_sysctls; }

protected:
    virtual void processLine(const std::vector<std::string> &tokens) override;

private:
    void processMount(std::vector<std::string>::const_iterator it, std::vector<std::string>::const_iterator end);
    void processUnmount(std::vector<std::string>::const_iterator it, std::vector<std::string>::const_iterator end);
    void processRun(std::vector<std::string>::const_iterator it, std::vector<std::string>::const_iterator end);
    void processSysctl(std::vector<std::string>::const_iterator it, std::vector<std::string>::const_iterator end);

    std::vector<Mount> m_mounts;
    std::vector<Unmount> m_unmounts;
    std::vector<Run> m_runs;
    std::vector<Sysctl> m_sysctls;
};

#endif
