#include <unistd.h>
#include <stdio.h>
#include <sys/reboot.h>
#include <sys/wait.h>
#include <signal.h>

#include "InitConfiguration.h"

struct TrackedProcess {
    const InitConfiguration::Run *run;
    pid_t currentPid;
};

static void alarmHandler(int signal) {
    (void)signal;
}

int main(int argc, char **argv) {
    if(getpid() != 1) {
        fputs("init must be run as pid 1\n", stderr);
        return 1;
    }

    InitConfiguration config;
    std::vector<TrackedProcess> trackedProcesses;

    try {
        config.parse("/etc/init.conf");
    } catch(const std::exception &e) {
        fprintf(stderr, "init: unable to parse /etc/init.conf: %s\n", e.what());
        return 1;
    }

    sigset_t watchedSignals;
    sigemptyset(&watchedSignals);
    sigaddset(&watchedSignals, SIGUSR1);
    sigaddset(&watchedSignals, SIGUSR2);
    sigaddset(&watchedSignals, SIGTERM);
    sigaddset(&watchedSignals, SIGCHLD);

    try {
        for(const auto &mount: config.mounts()) {
            mount.execute();
        }    

        for(const auto &sysctl: config.sysctls()) {
            sysctl.execute();
        }

        sigprocmask(SIG_BLOCK, &watchedSignals, nullptr);

        if(reboot(RB_DISABLE_CAD) < 0)
            throw std::runtime_error("reboot(RB_DISABLE_CAD) failed");

        trackedProcesses.reserve(config.runs().size());

        for(const auto &run: config.runs()) {
            TrackedProcess process;
            process.run = &run;
            process.currentPid = run.execute();
            trackedProcesses.emplace_back(std::move(process));
        }

    } catch(const std::exception &e) {
        fprintf(stderr, "init: error during system initialization: %s\n", e.what());
        return 1;
    }

    siginfo_t info;

    int rebootCode = -1;

    do {
        int result = sigwaitinfo(&watchedSignals, &info);
        if(result < 0 && errno == EINTR)
            continue;

        if(result < 0) {
            perror("sigwaitinfo");
            break;
        }

        switch(info.si_signo) {
        case SIGUSR1:
            rebootCode = RB_HALT_SYSTEM;
            break;

        case SIGUSR2:
            rebootCode = RB_POWER_OFF;
            break;

        case SIGTERM:
            rebootCode = RB_AUTOBOOT;
            break;

        case SIGCHLD:
            {
                while(true) {
                    info.si_pid = 0;
                    int result = waitid(P_ALL, 0, &info, WEXITED | WNOHANG);
                    if(result < 0) {
                        if(errno == EINTR)
                            continue;

                        if(errno == ECHILD)
                            break;

                        perror("waitid");
                        return 1;
                    }

                    if(info.si_pid == 0)
                        break;

                    for(auto &tracked: trackedProcesses) {
                        if(tracked.currentPid == info.si_pid) {
                            tracked.currentPid = tracked.run->execute();
                            break;
                        }
                    }
                }
            }
            break;
        }
    } while(rebootCode == -1);
    
    try {
        if(kill(-1, SIGTERM) < 0) {
            throw std::runtime_error("kill(SIGTERM) failed");
        }

        siginfo_t info;

        int result;

        struct sigaction act;
        act.sa_handler = alarmHandler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGALRM, &act, nullptr);

        if(alarm(10) < 0) {
            throw std::runtime_error("alarm() failed");
        }

        while(true) {
            result = waitid(P_ALL, 0, &info, WEXITED);
            if(result < 0) {
                if(errno == ECHILD || errno == EINTR)
                    break;
                else
                    throw std::runtime_error("waitid() failed");
            }
        } 

        if(alarm(0) < 0) {
            throw std::runtime_error("alarm() failed");
        }

         if(kill(-1, SIGKILL) < 0) {
            throw std::runtime_error("kill(SIGKILL) failed");
        }

        for(const auto &unmount: config.unmounts()) {
            unmount.execute();
        }

        if(reboot(rebootCode) < 0)
            throw std::runtime_error("reboot() failed");

    } catch(const std::exception &e) {
        fprintf(stderr, "init: error during system shutdown: %s\n", e.what());
        return 1;
    }

    return 0;
}
