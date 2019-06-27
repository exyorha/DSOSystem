#include <DSO/ApplicationInstance.h>

#if defined(__FreeBSD__)
#include <FreeBSDPlatform/FreeBSDDSOPlatform.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#endif

#if defined(_WIN32)
#include <Win32/WindowsDSOPlatform.h>
#endif


int main(int argc, char **argv) {
#if defined(__FreeBSD__)
    int console = open("/dev/console", O_RDWR | O_NOCTTY);
    if(console >= 0) {
        if(console != STDIN_FILENO)
            dup2(console, STDIN_FILENO);

        if(console != STDOUT_FILENO)
            dup2(console, STDOUT_FILENO);

        if(console != STDERR_FILENO)
            dup2(console, STDERR_FILENO);

        if(console != STDIN_FILENO && console != STDOUT_FILENO && console != STDERR_FILENO)
            close(console);
    }

    try {
        ApplicationInstance instance(std::unique_ptr<FreeBSDDSOPlatform>(new FreeBSDDSOPlatform()));

        instance.exec();
    } catch(const std::exception &e) {
        fprintf(stderr, "Uncaught exception, shutting down: %s\n", e.what());

        return 16;
    }
#endif

#if defined(_WIN32)
	ApplicationInstance instance(std::make_unique<WindowsDSOPlatform>());

	instance.exec();
#endif
}
