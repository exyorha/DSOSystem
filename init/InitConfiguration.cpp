#include "InitConfiguration.h"

#include <stdio.h>
#include <stdexcept>
#include <sstream>
#include <sys/mount.h>
#include <sys/stat.h>
#include <spawn.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

InitConfiguration::InitConfiguration() {

}

InitConfiguration::~InitConfiguration() {

}

void InitConfiguration::processLine(const std::vector<std::string> &tokens) {
    if(tokens.empty())
        return;

    auto it = tokens.begin();
    const auto &command = *it++;

    if(command == "MOUNT") {
        processMount(it, tokens.end());
    } else if(command == "UNMOUNT") {
        processUnmount(it, tokens.end());
    } else if(command == "RUN") {
        processRun(it, tokens.end());
    } else if(command == "SYSCTL") {
        processSysctl(it, tokens.end());
    } else {
        std::stringstream error;
        error << "Unsupported command token: " << command;
        throw std::runtime_error(error.str());
    }
}

void InitConfiguration::processMount(std::vector<std::string>::const_iterator it, std::vector<std::string>::const_iterator end) {
    static const struct {
        const char *token;
        unsigned int flag;
    } mountFlags[] = {
        { "BIND", MS_BIND },
        { "DIRSYNC", MS_DIRSYNC },
        { "MANDLOCK", MS_MANDLOCK },
        { "MOVE", MS_MOVE },
        { "NOATIME", MS_NOATIME },
        { "NODEV", MS_NODEV },
        { "NODIRATIME", MS_NODIRATIME },
        { "NOEXEC", MS_NOEXEC },
        { "NOSUID", MS_NOSUID },
        { "RDONLY", MS_RDONLY },
        { "RELATIME", MS_RELATIME },
        { "STRICTATIME", MS_STRICTATIME },
        { "REMOUNT", MS_REMOUNT },
        { "SILENT", MS_SILENT },
        { "SYNCHRONOUS", MS_SYNCHRONOUS }
    };

    Mount mount;

    if(it == end)
        throw std::runtime_error("MOUNT: device expected");

    mount.device = *it++;

    if(it == end)
        throw std::runtime_error("MOUNT: filesystem type expected");

    mount.filesystemType = *it++;

    if(it == end)
        throw std::runtime_error("MOUNT: target expected");

    mount.target = *it++;

    mount.mountOptions = 0;
    mount.mkdir = false;

    for(; it != end; it++) {
        const auto &token = *it;

        bool found = false;

        for(const auto &flag : mountFlags) {
            if(token == flag.token) {
                mount.mountOptions |= flag.flag;
                found = true;
                break;
            }
        }

        if(!found) {
            if(token == "MKDIR") {
                mount.mkdir = true;
            } else {
                throw std::runtime_error("MOUNT: token expected");
            }
        }
    }

    m_mounts.emplace_back(std::move(mount));
}

void InitConfiguration::processUnmount(std::vector<std::string>::const_iterator it, std::vector<std::string>::const_iterator end) {
    static const struct {
        const char *token;
        unsigned int flag;
    } unmountFlags[] = {
        { "FORCE", MNT_FORCE },
        { "DETACH", MNT_DETACH },
        { "EXPIRE", MNT_EXPIRE },
        { "NOFOLLOW", UMOUNT_NOFOLLOW }
    };

    Unmount unmount;

    if(it == end)
        throw std::runtime_error("UNMOUNT: mountpoint expected");

    unmount.target = *it++;
    unmount.flags = 0;

    for(; it != end; it++) {
        const auto &token = *it;

        bool found = false;

        for(const auto &flag : unmountFlags) {
            if(token == flag.token) {
                unmount.flags |= flag.flag;
                found = true;
                break;
            }
        }

        if(!found) {
            throw std::runtime_error("UNMOUNT: token expected");
        }
    }

    m_unmounts.emplace_back(std::move(unmount));
}

void InitConfiguration::processRun(std::vector<std::string>::const_iterator it, std::vector<std::string>::const_iterator end) {
    Run run;

    if(it == end)
        throw std::runtime_error("RUN: program expected");

    run.program = *it++;

    run.arguments.assign(it, end);

    m_runs.emplace_back(std::move(run));
}

void InitConfiguration::processSysctl(std::vector<std::string>::const_iterator it, std::vector<std::string>::const_iterator end) {
    Sysctl sysctl;

    if(it == end)
        throw std::runtime_error("SYSCTL: key expected");

    sysctl.key = *it++;

    if(it == end)
        throw std::runtime_error("SYSCTL: value expected");

    sysctl.value = *it++;

    if(it != end)
        throw std::runtime_error("SYSCTL: extra tokens");

    m_sysctls.emplace_back(std::move(sysctl));
}

void InitConfiguration::Mount::execute() const {
    if(mkdir) {
        if(::mkdir(target.c_str(), 0755) < 0) {
            throw std::runtime_error("mkdir() failed");
        }
    }

    if(::mount(device.c_str(), target.c_str(), filesystemType.c_str(), mountOptions, nullptr) < 0) {
        throw std::runtime_error("mount() failed");
    }
}

void InitConfiguration::Unmount::execute() const {
    if(umount2(target.c_str(), flags) < 0) {
        perror(target.c_str());
        throw std::runtime_error("umount() failed");
    }
}

pid_t InitConfiguration::Run::execute() const {
    pid_t pid;

    struct ActionWatcher {
        posix_spawn_file_actions_t actions;

        ActionWatcher() {
            posix_spawn_file_actions_init(&actions);
        }

        ~ActionWatcher() {
            posix_spawn_file_actions_destroy(&actions);
        }
    } fileActions;

    struct AttrWatcher {
        posix_spawnattr_t attrs;

        AttrWatcher() {
            posix_spawnattr_init(&attrs);
        }

        ~AttrWatcher() {
            posix_spawnattr_destroy(&attrs);
        }
    } spawnAttrs;

    sigset_t defset, maskset;
    sigemptyset(&defset);
    sigemptyset(&maskset);
    sigaddset(&defset, SIGUSR1);
    sigaddset(&defset, SIGUSR2);
    sigaddset(&defset, SIGTERM);
    sigaddset(&defset, SIGCHLD);
    posix_spawnattr_setsigdefault(&spawnAttrs.attrs, &defset);
    posix_spawnattr_setflags(&spawnAttrs.attrs, 
        POSIX_SPAWN_RESETIDS |
        POSIX_SPAWN_SETSIGDEF |
        POSIX_SPAWN_SETSIGMASK |
        POSIX_SPAWN_USEVFORK);
    posix_spawnattr_setsigmask(&spawnAttrs.attrs, &maskset);

    std::vector<char *> argv;
    argv.reserve(arguments.size() + 2);
    argv.push_back(const_cast<char *>(program.c_str()));
    for(const auto &argument: arguments) {
        argv.push_back(const_cast<char *>(argument.c_str()));
    }
    argv.push_back(nullptr);

    if(posix_spawn(&pid,
        program.c_str(),
        &fileActions.actions,
        &spawnAttrs.attrs,
        argv.data(),
        environ) < 0) {
        throw std::runtime_error("posix_spawn() failed");
    }

    return pid;
}

void InitConfiguration::Sysctl::execute() const {
    std::stringstream path;
    path << "/proc/sys/";

    for(auto ch: key) {
        if(ch == '.')
            path << '/';
        else   
            path << ch;
    }

    int fd = open(path.str().c_str(), O_WRONLY);
    if(fd < 0) {
        throw std::runtime_error("sysctl::open failed");
    }

    int result = write(fd, value.c_str(), value.size());
    close(fd);

    if(result < 0) {
        throw std::runtime_error("sysctl::write failed");
    }
}